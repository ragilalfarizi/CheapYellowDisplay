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

TEST_CASE("deserialize JSON and add Person_t to array", "[person]") {
  /*TEST_ASSERT_EQUAL(ESP_OK, allocate_person_dynamically(&persons,
   * &capacity));*/

  Person_t    p_new;
  const char* json_test_str =
      "{\"name\": \"ragil\", \"id\": 12, \"pos_x\": 50, \"pos_y\": 60}";

  /*const char* another_json_test_str =*/
  /*    "{" name ":" Ibnu "," id ":2," pos_x ":295," pos_y ":59}";*/

  printf("%s\n", json_test_str);

  // Parsing start
  TEST_ASSERT_EQUAL(
      0, json_parse_start(&jctx, json_test_str, strlen(json_test_str)));

  p_new = deserialize_person(&jctx, json_test_str);

  json_parse_end(&jctx);
  // parsing end

  /*add_person(&persons, &p_new, &current_size);*/

  TEST_ASSERT_EQUAL_STRING("ragil", p_new.name);
  TEST_ASSERT_EQUAL_INT(12, p_new.id);
  TEST_ASSERT_EQUAL_INT(50, p_new.pos_x);
  TEST_ASSERT_EQUAL_INT(60, p_new.pos_y);

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

/* ON PROGRESS. NOT YET IMPLEMENTED */
TEST_CASE("get data from serialized queue, convert it, and put in person queue",
          "[person]") {
  QueueHandle_t serialized_json_data_queue = xQueueCreate(5, SIZE_OF_BLE_MTU);
  TEST_ASSERT_NOT_NULL(serialized_json_data_queue);

  QueueHandle_t person_data_q = xQueueCreate(5, sizeof(Person_t));
  TEST_ASSERT_NOT_NULL(person_data_q);

  const char* s1 =
      "{\"name\": \"ragil\", \"id\": 12, \"pos_x\": 50, \"pos_y\": 60}";

  const char* s2 =
      "{\"name\": \"barkowi\", \"id\": 19, \"pos_x\": -15, \"pos_y\": -20}";

  const char* s3 =
      "{\"name\": \"tara\", \"id\": 39, \"pos_x\": -11, \"pos_y\": 70}";

  TEST_ASSERT_EQUAL(pdPASS,
                    xQueueSend(serialized_json_data_queue, &s1, portMAX_DELAY));
  TEST_ASSERT_EQUAL(pdPASS,
                    xQueueSend(serialized_json_data_queue, &s2, portMAX_DELAY));
  TEST_ASSERT_EQUAL(pdPASS,
                    xQueueSend(serialized_json_data_queue, &s3, portMAX_DELAY));

  TEST_ASSERT_EQUAL(3, uxQueueMessagesWaiting(serialized_json_data_queue));

  Person_t temp_person;

  // while serialize queue is available
  while (uxQueueMessagesWaiting(serialized_json_data_queue) > 0) {
    char data[128];

    TEST_ASSERT_EQUAL(
        pdPASS, xQueueReceive(serialized_json_data_queue, data, portMAX_DELAY));

    temp_person = deserialize_person(&jctx, data);

    TEST_ASSERT_EQUAL(pdPASS, xQueueSend(person_data_q, data, portMAX_DELAY));
  }

  TEST_ASSERT_EQUAL(3, uxQueueMessagesWaiting(person_data_q));
}
