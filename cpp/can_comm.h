#pragma once

#include <iostream>
#include <functional>
#include <algorithm>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <stdexcept>
#include <cstring>
#include <linux/can/raw.h>
#include <linux/can/bcm.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <net/if.h>
#include <errno.h>

using namespace std::placeholders;
using CAN_interface = std::string;
using CAN_frame = struct can_frame;
using CAN_stamp = struct timeval;
using CAN_callback = std::function<void(CAN_frame&, CAN_stamp&)>;


class CAN_Comm
{
public:
    CAN_Comm(CAN_interface interface, CAN_callback callback)
    {
        connect(interface);
        add_callback(callback);
        create_thread();
    }

    ~CAN_Comm()
    {
        thread_is_running.store(false, std::memory_order_relaxed);
        thread_.join();
    }

    void add_callback(CAN_callback callback)
    {
        this->callbacks.push_back(callback);
    }

    void send_message(CAN_frame &message)
    {
        write(socket_, &message, sizeof(CAN_frame));
    }

private:
    void throw_with_errno(const std::string& where)
    {
        throw std::runtime_error(where + ", errno=" + std::to_string(errno) +
            ":" + strerror(errno) + "\n");
    }

    void connect(CAN_interface interface)
    {
        // Create socket
        socket_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if(socket_ == -1)
            throw_with_errno("Creating CAN_BCM socket on can interface" + interface);
        
        // Configure timeout
        struct timeval tv;
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        auto ret = setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        if(ret == -1)
            throw_with_errno("Setting CAN_BCM receive timeout");

        struct ifreq ifr;
        strcpy(ifr.ifr_name, interface.c_str());
        ret = ioctl(socket_, SIOCGIFINDEX, &ifr);
        if(ret == -1)
            throw_with_errno("Getting interface index " + interface);
        
        struct sockaddr_can addr;
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        ret = bind(socket_, (struct sockaddr*)&addr, sizeof(addr));
        if(ret == -1)
            throw_with_errno("Binding address");
    }

    void create_thread()
    {
        thread_is_running.store(true, std::memory_order_relaxed);
        thread_ = std::thread(&CAN_Comm::run_reader, this);
    }

    void run_reader()
    {
        while(thread_is_running.load())
        {
            CAN_frame frame;
            auto bytes = read(socket_, &frame, sizeof(frame));
            if(bytes != -1)
            {
                struct timeval tv;
                ioctl(socket_, SIOCGSTAMP_OLD, &tv);
                invoke_callbacks(frame, tv);
            }
        }
    }

    void invoke_callbacks(CAN_frame &frame, CAN_stamp &stamp)
    {
        for(auto &func : callbacks)
            func(frame, stamp);
    }

    CAN_Comm() = delete;

    int socket_;
    std::vector<CAN_callback> callbacks;
    std::thread thread_;
    mutable std::mutex mutex_;
    std::atomic<bool> thread_is_running;
};

