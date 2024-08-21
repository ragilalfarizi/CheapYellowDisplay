#include "esp_err.h"
#include "esp_log.h"
#include "person_handling.h"
#include "unity.h"

// Person_t* persons;
uint8_t        capacity     = INITIAL_PERSON_CAPACITY;
uint8_t        size         = 0;
static uint8_t current_size = 0;
jparse_ctx_t   jctx;

Person_t* persons;

TEST_CASE("i2c init", "[person][comm]") {
  TEST_ASSERT_EQUAL(ESP_OK, i2c_init());
}

TEST_CASE("memory allocated dynamically for Person", "[person]") {
  TEST_ASSERT_EQUAL(ESP_OK, allocate_person_dynamically(&persons, &capacity));
}

TEST_CASE("add persons into dynamic memory", "[person]") {
  TEST_ASSERT_EQUAL(ESP_OK, allocate_person_dynamically(&persons, &capacity));

  Person_t p1 = {111, "ragil", 90, 80};
  Person_t p2 = {222, "ahmad", -20, -70};

  add_person(&persons, &p1, &current_size);
  printf("Person 1 ID: %u\n", persons[0].id);
  printf("Person 1 Name: %s\n", persons[0].name);
  printf("Person 1 X: %d\n", persons[0].pos_x);
  printf("Person 1 Y: %d\n", persons[0].pos_y);

  TEST_ASSERT_EQUAL_UINT16(111, persons[0].id);
  TEST_ASSERT_EQUAL_STRING("ragil", persons[0].name);
  TEST_ASSERT_EQUAL_INT16(90, persons[0].pos_x);
  TEST_ASSERT_EQUAL_INT16(80, persons[0].pos_y);

  add_person(&persons, &p2, &current_size);
  printf("Person 2 ID: %u\n", persons[1].id);
  printf("Person 2 Name: %s\n", persons[1].name);
  printf("Person 2 X: %d\n", persons[1].pos_x);
  printf("Person 2 Y: %d\n", persons[1].pos_y);

  TEST_ASSERT_EQUAL_UINT16(222, persons[1].id);
  TEST_ASSERT_EQUAL_STRING("ahmad", persons[1].name);
  TEST_ASSERT_EQUAL_INT16(-20, persons[1].pos_x);
  TEST_ASSERT_EQUAL_INT16(-70, persons[1].pos_y);

  free(persons);
  current_size = 0;
}

TEST_CASE("deserialize JSON and add Person_t to array", "[person]") {
  TEST_ASSERT_EQUAL(ESP_OK, allocate_person_dynamically(&persons, &capacity));

  Person_t    p_new;
  const char* json_test_str =
      "{\"name\": \"ragil\", \"id\": 12, \"pos_x\": 50, \"pos_y\": 60}";

  // Parsing start
  TEST_ASSERT_EQUAL(
      0, json_parse_start(&jctx, json_test_str, strlen(json_test_str)));

  p_new = deserialize_person(&jctx, json_test_str);

  json_parse_end(&jctx);
  // parsing end

  add_person(&persons, &p_new, &current_size);

  TEST_ASSERT_EQUAL_STRING("ragil", persons[0].name);
  TEST_ASSERT_EQUAL_INT(12, persons[0].id);
  TEST_ASSERT_EQUAL_INT(50, persons[0].pos_x);
  TEST_ASSERT_EQUAL_INT(60, persons[0].pos_y);

  free(persons);
}

TEST_CASE("serialized_data queue can be filled", "[person]") {
  const char* s1 =
      "{\"name\": \"ragil\", \"id\": 12, \"pos_x\": 50, \"pos_y\": 60}";

  const char* s2 =
      "{\"name\": \"barkowi\", \"id\": 19, \"pos_x\": -15, \"pos_y\": -20}";

  const char* s3 =
      "{\"name\": \"tara\", \"id\": 39, \"pos_x\": -11, \"pos_y\": 70}";

  QueueHandle_t serialized_json_data_queue;
  serialized_json_data_queue = xQueueCreate(10, sizeof(char*));

  TEST_ASSERT_NOT_NULL(serialized_json_data_queue);

  // sending to queue
  xQueueSend(serialized_json_data_queue, &s1, portMAX_DELAY);
  xQueueSend(serialized_json_data_queue, &s2, portMAX_DELAY);
  xQueueSend(serialized_json_data_queue, &s3, portMAX_DELAY);

  // check if the queue has items
  TEST_ASSERT_EQUAL(3, uxQueueMessagesWaiting(serialized_json_data_queue));

  char* received_item;

  // receiving and verifying items
  TEST_ASSERT_EQUAL(pdPASS, xQueueReceive(serialized_json_data_queue,
                                          &received_item, portMAX_DELAY));
  TEST_ASSERT_EQUAL_STRING(s1, received_item);

  TEST_ASSERT_EQUAL(pdPASS, xQueueReceive(serialized_json_data_queue,
                                          &received_item, portMAX_DELAY));
  TEST_ASSERT_EQUAL_STRING(s2, received_item);

  TEST_ASSERT_EQUAL(pdPASS, xQueueReceive(serialized_json_data_queue,
                                          &received_item, portMAX_DELAY));
  TEST_ASSERT_EQUAL_STRING(s3, received_item);

  vQueueDelete(serialized_json_data_queue);
}
