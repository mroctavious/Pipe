#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <cstring>

#ifndef PIPE_MAX_BUFFER_SIZE
    #define PIPE_MAX_BUFFER_SIZE 8192
#endif

/**
 * @brief Create a pipe easily, write and read with std::strings.
 * Also you can open pipes from another process just by passing the
 * process id and the file descriptors of the pipe.
 */
class Pipe{
    static const int buffer_size = PIPE_MAX_BUFFER_SIZE; //< Maximum size of the buffer where the data will be read.
    int pid; //< Process ID
    int write_fd; //< Write file descriptor.
    int read_fd; //< Read file descriptor.
    char buffer[buffer_size]; //< Buffer.
    constexpr static const char* format_str="/proc/%d/fd/%d"; //< Format of the string indicating the path to the write and read file descriptors of the process.

    /**
     * @brief Get the formated string to open file descriptors.
     * 
     * @param _pid PID of the program who created the pipe.
     * @param _fd file descriptor value.
     * @return std::string string with the path to the fd.
     */
    static std::string get_format_str(int _pid, int _fd){
        char buffer[64]={0};
        snprintf(buffer, sizeof(buffer), format_str, _pid, _fd);
        buffer[sizeof(buffer)-1]='\0';
        return std::string(buffer);
    }

    /**
     * @brief Open the pipe file descriptor.
     * 
     * @param _pid process id of the process who created the pipe.
     * @param _fd file descriptor to open.
     * @param _flags open flags, like read-only or write.
     * @return int the file descriptor.
     */
    static int open(int _pid, int _fd, int _flags){
        auto path = get_format_str(_pid, _fd);
        int fd = ::open(path.c_str(), _flags);
        if(fd <= 0){
            std::string error="Error: Can't open pipe. Path: ";
            error += path;
            throw std::runtime_error(error);
        }
        return fd;
    }

    /**
     * @brief Create a pipe.
     *
     * @return std::pair<int,int> Read and write.
     */
    std::pair<int,int> create_pipe(){
        int fd[2]={-1};
        if(pipe(fd) != 0){
            throw std::runtime_error("Error while creating new pipe.");
        }
        return std::make_pair(fd[0],fd[1]);
    }

public:

    /**
     * @brief Destroy the Pipe object.
     * 
     */
    ~Pipe(){
        if(write_fd > 0) close(write_fd);
        if(read_fd > 0) close(read_fd);
    }

    /**
     * @brief create pipe and init object.
     * 
     */
    void init(){
        auto fds = create_pipe();
        read_fd = fds.first;
        write_fd = fds.second;
        pid = getpid();
    }

    /**
     * @brief Construct a new Pipe object
     * 
     */
    Pipe():write_fd{-1}, read_fd{-1}, pid(-1){
    }

    /**
     * @brief Construct a new Pipe object.
     * 
     * @param _pid process id of the process who created the pipe.
     * @param _write_fd write file descriptor.
     */
    Pipe(int _pid, int _write_fd):pid{_pid}, write_fd{-1}, read_fd{-1}{
        try{
            write_fd = this->open(pid, _write_fd, O_WRONLY);
        }
        catch(const std::exception &exp){
            std::cout << exp.what() << std::endl;
        }
    }

    /**
     * @brief Construct a new Pipe object.
     * 
     * @param _pid process id of the process who created the pipe.
     * @param _write_fd write file descriptor.
     */
    static std::string read(int _pid, int _read_fd){
        int rfd = -1;
        try{
            char buff[buffer_size]={0};
            rfd = Pipe::open(_pid, _read_fd, O_RDONLY);
            if( rfd <= 0 ) throw std::runtime_error("Cannot read, the pipe read file descriptor is not initialized.");
            std::string result;
            std::memset(buff, 0, buffer_size);
            auto bytes = ::read(rfd, buff, buffer_size);
            if( bytes > 0 ){
                result = std::string(buff, bytes);
            }
            close(rfd);
            return result;            
        }
        catch(const std::exception &exp){
            std::cout << exp.what() << std::endl;
        }
        if (rfd > 0) close(rfd);
        return {};
    }

    /**
     * @brief Write into a process pipe.
     * 
     * @param _pid process id of the process who created the pipe.
     * @param _write_fd write file descriptor.
     * @param data data to be written in the pipe.
     */
    static int write(int _pid, int _write_fd, std::string data){
        int wfd = -1;
        int bytes = -1;
        try{
            wfd = Pipe::open(_pid, _write_fd, O_WRONLY);
            if( wfd <= 0 ) throw std::runtime_error("Cannot open pipe, the pipe write file descriptor is not initialized.");
            bytes = ::write(wfd, data.c_str(), data.size());
        }
        catch(const std::exception &exp){
            std::cout << exp.what() << std::endl;
        }
        if (wfd > 0) close(wfd);
        return bytes;
    }


    /**
     * @brief read data from pipe.
     * 
     * @return std::string data from pipe.
     */
    std::string read(){
        if( read_fd <= 0 ) throw std::runtime_error("Cannot read, the pipe read file descriptor is not initialized.");
        std::string result;
        std::memset(buffer, 0, buffer_size);
        auto bytes = ::read(read_fd, buffer, buffer_size);
        if( bytes > 0 ){
            result = std::string(buffer, bytes);
        }
        return result;
    }

    /**
     * @brief write data into the pipe.
     * 
     * @param data data to be written.
     * @return int size of bytes written.
     */
    int write(std::string data){
        if( write_fd <= 0 ) throw std::runtime_error("Cannot write, the pipe write file descriptor is not initialized.");
        auto bytes = ::write(write_fd, data.c_str(), data.size());
        return bytes;
    }

    /**
     * @brief print current_pipe.
     *
     */
    void print(){
        std::cout << "pid: " << pid << std::endl; ;
        std::cout << "write_fd: " << write_fd << std::endl; ;
        std::cout << "read_fd: " << read_fd << std::endl; ;
    }

    /**
     * @brief Get the process id.
     *
     */
    auto get_pid(){
        return pid;
    }

    /**
     * @brief Get pipe write file descriptor.
     *
     */
    auto get_write_fd(){
        return write_fd;
    }

    /**
     * @brief Get pipe read file descriptor.
     *
     */
    auto get_read_fd(){
        return read_fd;
    }

};

