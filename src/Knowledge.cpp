#include "knowledge_tools/Knowledge.hpp"

// ============================================================
// Constructor
// ============================================================

Knowledge::Knowledge(const rclcpp::Node::SharedPtr &node) : node_(node) {

  cb_group_clients_ =
      node_->create_callback_group(rclcpp::CallbackGroupType::Reentrant);

  const rclcpp::QoS qos = rclcpp::ServicesQoS();

  get_world_state_client_ =
      node_->create_client<knowledge_interfaces::srv::GetWorldState>(
          "/knowledge/knowledge_manager/world/get_state", qos,
          cb_group_clients_);

  update_world_state_client_ =
      node_->create_client<knowledge_interfaces::srv::UpdateWorldState>(
          "/knowledge/knowledge_manager/world/update_state", qos,
          cb_group_clients_);

  get_phrase_client_ =
      node_->create_client<knowledge_interfaces::srv::GetPhrase>(
          "/knowledge/knowledge_manager/phrases/get_phrase", qos,
          cb_group_clients_);
}

// ============================================================
// GetWorldState
// ============================================================

json Knowledge::get_world_state(const std::string &category,
                                const std::string &id,
                                const std::string &attribute,
                                std::chrono::seconds timeout) {

  auto request =
      std::make_shared<knowledge_interfaces::srv::GetWorldState::Request>();

  request->category = category;
  request->id = id;
  request->attribute = attribute;

  json result;

  bool completed = call_service<knowledge_interfaces::srv::GetWorldState>(
      get_world_state_client_, request,
      [&](auto response) {
        try {
          result = json::parse(response->json);
        } catch (const json::parse_error &e) {
          RCLCPP_ERROR(node_->get_logger(),
                       "Failed to parse world state JSON: %s", e.what());
        }
      },
      timeout, "GetWorldState");

  if (!completed) {
    return json{};
  }

  return result;
}

// ============================================================
// UpdateWorldState
// ============================================================

bool Knowledge::update_world_state(const json &update,
                                   std::chrono::seconds timeout) {

  auto request =
      std::make_shared<knowledge_interfaces::srv::UpdateWorldState::Request>();

  request->json = update.dump(2);

  bool success = false;

  bool completed = call_service<knowledge_interfaces::srv::UpdateWorldState>(
      update_world_state_client_, request,
      [&](auto response) {
        success = response->success;

        RCLCPP_INFO(node_->get_logger(), "UpdateWorldState %s: %s",
                    success ? "OK" : "FAILED", response->message.c_str());
      },
      timeout, "UpdateWorldState");

  return completed && success;
}

// ============================================================
// GetPhrase
// ============================================================

std::string Knowledge::get_phrase(const std::string &phrase,
                                  const std::string &context,
                                  std::chrono::seconds timeout) {

  auto request =
      std::make_shared<knowledge_interfaces::srv::GetPhrase::Request>();

  request->phrase = phrase;
  request->context = context;

  std::string text;

  call_service<knowledge_interfaces::srv::GetPhrase>(
      get_phrase_client_, request,
      [&](auto response) {
        if (response->success) {
          text = response->text;
        }
      },
      timeout, "GetPhrase");

  return text;
}
