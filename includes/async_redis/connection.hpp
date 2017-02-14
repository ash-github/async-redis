#pragma once

#include <queue>
#include <functional>
#include <memory>
#include <tuple>

#include <async_redis/details/connector.hpp>
#include <async_redis/parser/base_resp_parser.h>

using namespace libevpp;


namespace async_redis
{
  class connection : public details::connector<1024, connection>
  {
    friend details::connector<1024, connection>;

  public:
    using parser_t        = parser::base_resp_parser::parser;
    using reply_cb_t      = std::function<void (parser_t&)>;

    connection(event_loop::event_loop_ev& event_loop);

    void disconnect();
    bool pipelined_send(std::string&& pipelined_cmds, std::vector<reply_cb_t>&& callbacks);
    bool send(const std::string&& command, const reply_cb_t& reply_cb);

  private:
    void data_received(const char* data, ssize_t len);

  private:
    std::queue<std::tuple<reply_cb_t, parser_t>> req_queue_;
  };
}
