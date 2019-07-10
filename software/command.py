import json

class CommandTypes(object):
	COMM_ENABLE = 'enabled'
	COMM_DISABLE = 'disabled'
	COMM_START = 'start'
	COMM_STOP = 'stop'
	COMM_SET = 'set'

class CommandCreate(object):
	def create(type_, value = None):
		if type_ == CommandTypes.COMM_SET:
			return json.dumps({'type': type_, 'value' : value})
		else:
			return json.dumps({'type' : type_})

class CommandParse(object):
	def parse(json_str):
		return json.loads(json_str)

