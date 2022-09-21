import websocket # pip install websocket-client
import _thread # for websocket-client
import time # for websocket-client
import rel # for websocket-client
from time import sleep

def on_message(ws, message):
    print(message)
    if message == '?':
        ws.send('close: serial8266wifi001\n''tell me a joke\n')
    if message == 'close: serial8266wifi001\n':
        ws.send('serial8266wifi001 : Closing\n')
    if message == 'close: serial8266wifi001':
        ws.send('serial8266wifi001 : Closing\n')
        ws.close()
    if message == ('tell me a joke'):
        ws.send('How many programmers does it take to change a light bulb?\n')
        sleep(4)
        ws.send('None – It’s a hardware problem\n')
        
def on_error(ws, error):
    ws.send('serial8266wifi001 : Error')
    print(error)

def on_close(ws, close_status_code, close_msg):
    ws.send('serial8266wifi001 : Closed')
    print("### closed ###")

def on_open(ws):
    ws.send('serial8266wifi001 : Opened')
    print("Opened connection")

if __name__ == "__main__":
    websocket.enableTrace(False) # enables debug messages
    ws = websocket.WebSocketApp("ws://172.16.0.99:80/webserial",
                              on_open=on_open,
                              on_message=on_message,
                              on_error=on_error,
                              on_close=on_close)


    ws.run_forever(dispatcher=rel)  # Set dispatcher to automatic reconnection
    rel.signal(2, rel.abort)  # Keyboard Interrupt
    rel.dispatch()
