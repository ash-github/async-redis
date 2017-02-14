#pragma once

#include <libevpp/event_loop/event_loop_ev.h>
#include <libevpp/network/tcp_socket.hpp>
#include <libevpp/network/unix_socket.hpp>


namespace async_redis
{
namespace details {

  template<int BufferSize, typename Handler>
  class connector
  {
    using async_socket    = libevpp::network::async_socket;
    using tcp_socket      = libevpp::network::tcp_socket;
    using unix_socket     = libevpp::network::unix_socket;

  public:
    inline
    connector(libevpp::event_loop::event_loop_ev& event_loop)
      : event_loop_(event_loop)
    { }

    void connect(async_socket::connect_handler_t&& handler, const std::string& ip, int port)
    {
      if (!socket_ || !socket_->is_valid())
        socket_ = std::make_unique<tcp_socket>(event_loop_);

      static_cast<tcp_socket&>(*socket_).async_connect(ip, port, handler);
    }

    void connect(async_socket::connect_handler_t&& handler, const std::string& path)
    {
      if (!socket_ || !socket_->is_valid())
        socket_ = std::make_unique<unix_socket>(event_loop_);

      static_cast<unix_socket&>(*socket_).async_connect(path, handler);
    }

    inline
    bool is_connected() const {
      return socket_ && socket_->is_connected();
    }

    inline
    void disconnect() {
      socket_->close();
    }

  protected:

    inline
    void do_read() {
      socket_->async_read(data_, BufferSize, std::bind(&Handler::data_received, (Handler*)this, (const char*)data_, std::placeholders::_1));
    }

    inline
    bool do_write(const std::string& data, libevpp::network::async_socket::ready_cb_t&& cb) {
      return socket_->async_write(std::move(data), std::move(cb));
    }


  private:
    libevpp::event_loop::event_loop_ev& event_loop_;
    char data_[BufferSize];

    std::unique_ptr<async_socket> socket_;
  };

}
}
