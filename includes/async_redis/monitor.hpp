#pragma once

#include <async_redis/details/connector.hpp>
#include <async_redis/parser/base_resp_parser.h>

#include <unordered_map>
#include <list>
#include <string>

using std::string;
using namespace libevpp;

namespace async_redis
{
  class monitor : public details::connector<1024, monitor>
  {
    friend details::connector<1024, monitor>;

  public:
    enum EventState {
      Sub,
      Unsub,
      Stream,
      Disconnected
    };

    using parser_t     = parser::base_resp_parser::parser;
    using watcher_cb_t = std::function<void (const string&, parser_t&, EventState)>;

    monitor(event_loop::event_loop_ev &event_loop);

    bool is_watching() const;
    void disconnect();

    bool psubscribe(const std::list<string>& channels, watcher_cb_t&& cb);
    bool subscribe(const std::list<string>& channels, watcher_cb_t&& cb);
    bool unsubscribe(const std::list<string>& channels, watcher_cb_t&& cb);
    bool punsubscribe(const std::list<string>& channels, watcher_cb_t&& cb);

  private:
    bool send_and_receive(string&& data);
    void handle_message_event(parser_t& channel, parser_t& value);
    void handle_subscribe_event(parser_t& channel, parser_t& clients);
    void handle_psubscribe_event(parser_t& channel, parser_t& clients);
    void handle_punsubscribe_event(parser_t& pattern, parser_t& clients);
    void handle_unsubscribe_event(parser_t& channel, parser_t& clients);
    void handle_pmessage_event(parser_t& pattern, parser_t& channel, parser_t& value);
    void handle_event(parser_t&& request);
    void report_disconnect();

    void data_received(const char *data, ssize_t len);

  private:
    parser_t parser_;
    std::unordered_map<std::string, watcher_cb_t> watchers_;
    std::unordered_map<std::string, watcher_cb_t> pwatchers_;

    bool is_watching_ = false;
  };

}
