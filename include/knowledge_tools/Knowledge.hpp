#pragma once

#include <chrono>
#include <future>
#include <string>

#include <nlohmann/json.hpp>

#include <rclcpp/callback_group.hpp>
#include <rclcpp/node.hpp>

#include "knowledge_interfaces/srv/get_phrase.hpp"
#include "knowledge_interfaces/srv/get_world_state.hpp"
#include "knowledge_interfaces/srv/update_world_state.hpp"

using json = nlohmann::json;
using namespace std::chrono_literals;

class Knowledge {
public:
  explicit Knoeledge(const rclcpp::Node::SharedPtr &node);

  // ============================================================
  // World State
  // ============================================================

  json get_world_state(const std::string &category = "world",
                       const std::string &id = "",
                       const std::string &attribute = "",
                       std::chrono::seconds timeout = 5s);

  bool update_world_state(const json &update,
                          std::chrono::seconds timeout = 5s);

  // ============================================================
  // Phrases
  // ============================================================

  std::string get_phrase(const std::string &phrase, const std::string &context,
                         std::chrono::seconds timeout = 5s);

private:
  rclcpp::Node::SharedPtr node_;

  rclcpp::CallbackGroup::SharedPtr cb_group_clients_;

  // ============================================================
  // Service Clients
  // ============================================================

  rclcpp::Client<knowledge_interfaces::srv::GetWorldState>::SharedPtr
      get_world_state_client_;

  rclcpp::Client<knowledge_interfaces::srv::UpdateWorldState>::SharedPtr
      update_world_state_client_;

  rclcpp::Client<knowledge_interfaces::srv::GetPhrase>::SharedPtr
      get_phrase_client_;

  // ============================================================
  // Generic helper
  // ============================================================

  template <typename ServiceT, typename ResponseHandlerT>
  bool call_service(typename rclcpp::Client<ServiceT>::SharedPtr client,
                    typename ServiceT::Request::SharedPtr request,
                    ResponseHandlerT &&response_handler,
                    std::chrono::seconds timeout,
                    const std::string &service_name) {

    if (!client->wait_for_service(timeout)) {
      RCLCPP_ERROR(node_->get_logger(), "Service %s not available",
                   service_name.c_str());
      return false;
    }

    auto future = client->async_send_request(request);

    auto status = future.wait_for(timeout);

    if (status != std::future_status::ready) {
      RCLCPP_ERROR(node_->get_logger(), "Timeout calling service %s",
                   service_name.c_str());
      return false;
    }

    response_handler(future.get());

    return true;
  }
};
