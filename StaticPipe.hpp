#pragma once
#include <Pipe.hpp>

/**
 * @brief This class creates a singleton pipe so it does not open and closes in each operation.
 * 
 */
class StaticPipe{
private:
    Pipe pipe; //< Pipe object.

    /**
     * @brief Construct a new Static Pipe object.
     * 
     */
    StaticPipe(){ 
        pipe.init();
    }

    /**
     * @brief Get the pipe object.
     * 
     * @return Pipe& object.
     */
    Pipe& get_pipe(){
        return pipe;
    }

public:
    /**
     * @brief Delete public construction object.
     * 
     */
    StaticPipe(const StaticPipe&) = delete;

    /**
     * @brief Delete public construction object.
     * 
     */
    StaticPipe& operator=(const StaticPipe &) = delete;

    /**
     * @brief Delete public construction object.
     * 
     */
    StaticPipe(StaticPipe &&) = delete;

    /**
     * @brief Delete public construction object.
     * 
     */
    StaticPipe & operator=(StaticPipe &&) = delete;

    /**
     * @brief Get the object instance.
     * 
     * @return auto& 
     */
    static auto& instance(){
        static StaticPipe spipe;
        return spipe;
    }

    /**
     * @brief Read data from pipe.
     * 
     * @return auto data.
     */
    static auto read(){
        auto& spipe = StaticPipe::instance();
        return spipe.get_pipe().read();
    }

    /**
     * @brief Get the pid object.
     * 
     * @return auto process id.
     */
    static auto get_pid(){
        auto& spipe = StaticPipe::instance();
        return spipe.get_pipe().get_pid();
    }

    /**
     * @brief Get the write fd object.
     * 
     * @return auto write file descriptor.
     */
    static auto get_write_fd(){
        auto& spipe = StaticPipe::instance();
        return spipe.get_pipe().get_write_fd();
    }

    /**
     * @brief Get the read fd object.
     * 
     * @return auto read file descriptor.
     */
    static auto get_read_fd(){
        auto& spipe = StaticPipe::instance();
        return spipe.get_pipe().get_read_fd();
    }

    /**
     * @brief Write data into the pipe.
     * 
     * @tparam T Class with .data() and .size() method.
     * @param data object to be written in the pipe.
     * @return auto number of bytes written.
     */
    template<typename T>
    static auto write(T& data){
        auto& spipe = StaticPipe::instance();
        return spipe.get_pipe().write(data);
    };

    /**
     * @brief Print current pipe values.
     * 
     */
    static void print(){
        auto& spipe = StaticPipe::instance();
        return spipe.get_pipe().print();
    }
};
