#ifndef CORPUS_H
#define CORPUS_H

#define NUMBER_NODE_NAMES (49)
#define NUMBER_ARG_NAMES (34)
#define BADKEY -1

/** Symbol of key => value */
typedef struct {
    char *key;
    int val;
} celery_kv_sym_t;

/* Begin Celery Node definition */
typedef enum CeleryNodeKind {
  NAMED_PIN = 0,
	NOTHING = 1,
	TOOL = 2,
	COORDINATE = 3,
	MOVE_ABSOLUTE = 4,
	MOVE_RELATIVE = 5,
	WRITE_PIN = 6,
	READ_PIN = 7,
	CHANNEL = 8,
	WAIT = 9,
	SEND_MESSAGE = 10,
	EXECUTE = 11,
	IF = 12,
	SEQUENCE = 13,
	HOME = 14,
	FIND_HOME = 15,
	ZERO = 16,
	EMERGENCY_LOCK = 17,
	EMERGENCY_UNLOCK = 18,
	READ_STATUS = 19,
	SYNC = 20,
	CHECK_UPDATES = 21,
	POWER_OFF = 22,
	REBOOT = 23,
	TOGGLE_PIN = 24,
	EXPLANATION = 25,
	RPC_REQUEST = 26,
	RPC_OK = 27,
	RPC_ERROR = 28,
	CALIBRATE = 29,
	PAIR = 30,
	REGISTER_GPIO = 31,
	UNREGISTER_GPIO = 32,
	CONFIG_UPDATE = 33,
	FACTORY_RESET = 34,
	EXECUTE_SCRIPT = 35,
	SET_USER_ENV = 36,
	TAKE_PHOTO = 37,
	POINT = 38,
	INSTALL_FARMWARE = 39,
	UPDATE_FARMWARE = 40,
	REMOVE_FARMWARE = 41,
	SCOPE_DECLARATION = 42,
	IDENTIFIER = 43,
	VARIABLE_DECLARATION = 44,
	PARAMETER_DECLARATION = 45,
	SET_SERVO_ANGLE = 46,
	CHANGE_OWNERSHIP = 47,
	INSTALL_FIRST_PARTY_FARMWARE = 48
} celery_node_kind_name_t;

/* Begin Celery Arg definition */
typedef enum CeleryArgKind {
  ELSE = 0,
	THEN = 1,
	LOCALS = 2,
	OFFSET = 3,
	PIN_NUMBER = 4,
	DATA_VALUE = 5,
	LOCATION = 6,
	LABEL = 7,
	MILLISECONDS = 8,
	PACKAGE = 9,
	PIN_VALUE = 10,
	RADIUS = 11,
	RHS = 12,
	URL = 13,
	VALUE = 14,
	VERSION = 15,
	X = 16,
	Y = 17,
	Z = 18,
	PIN_ID = 19,
	PIN_TYPE = 20,
	POINTER_ID = 21,
	POINTER_TYPE = 22,
	PIN_MODE = 23,
	SEQUENCE_ID = 24,
	LHS = 25,
	OP = 26,
	CHANNEL_NAME = 27,
	MESSAGE_TYPE = 28,
	TOOL_ID = 29,
	AXIS = 30,
	MESSAGE = 31,
	SPEED = 32,
	DATA_TYPE = 33
} celery_arg_kind_name_t;

celery_node_kind_name_t node_name_from_string(char *key);
char* string_from_node_name(celery_node_kind_name_t name);

celery_arg_kind_name_t arg_name_from_string(char *key);
char* string_from_arg_name(celery_arg_kind_name_t name);

#endif
