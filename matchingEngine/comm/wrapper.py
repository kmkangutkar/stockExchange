import sys

from ctypes import *

class new_obj(Structure):
    _fields_ = [
        ("handlInst", c_char_p),
        ("client_orderID", c_char_p),
        ("symbol", c_char_p),
        ("side", c_char_p),
        ("type",  c_char_p),
        ("price", c_double),
        ("quantity", c_double),
    ]

class can_obj(Structure):
    _fields_ = [
        ("client_orderID", c_char_p),
        ("symbol", c_char_p),
        ("side", c_char_p),
        ("quantity", c_double),
    ]

class rep_obj(Structure):
    _fields_ = [
        ("handlInst", c_char_p),
        ("client_orderID", c_char_p),
        ("symbol", c_char_p),
        ("side", c_char_p),
        ("original_orderID", c_char_p),
        ("type",  c_char_p),
        ("price", c_double),
        ("quantity", c_double),
    ]

def call_action_handler(request_type, message):
    
    if request_type == "new":
        func = CDLL('../libs/wrapper.dll').func_n
        obj = new_obj()
        obj.handlInst = message['handlInst']
        obj.client_orderID = message['client_orderID']
        obj.symbol = message['symbol']
        obj.side = message['side']
        obj.type = message['type']
        obj.price = message['price']
        obj.quantity = message['quantity']

        func.argtypes = [new_obj]
        func.restype = new_obj


    elif request_type == "cancel":
        func = CDLL('../libs/wrapper.dll').func_c
        obj = can_obj()
        obj.client_orderID = message['client_orderID']
        obj.symbol = message['symbol']
        obj.side = message['side']
        obj.quantity = message['quantity']

        func.argtypes = [can_obj]
        func.restype = can_obj
    
    elif request_type == "replace":
        func = CDLL('../libs/wrapper.dll').func_r
        obj = rep_obj()
        obj.handlInst = message['handlInst']
        obj.client_orderID = message['client_orderID']
        obj.symbol = message['symbol']
        obj.side = message['side']
        obj.original_orderID = message['original_orderID']
        obj.type = message['type']
        obj.price = message['price']
        obj.quantity = message['quantity']

        func.argtypes = [rep_obj]
        func.restype = rep_obj

    obj = func(obj)
    return True

def main():
    message = {
        "handlInst":'1',
        "client_orderID":'2333',
        "original_orderID":'Shashank',
        "symbol":'STBK',
        "side":'1',
        "type":'J',
        "price":2000.0,
        "quantity":5.0,
    }

    call_action_handler('replace', message)

if __name__ == '__main__':
    main()
