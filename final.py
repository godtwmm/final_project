from machine import Pin
from machine import UART
from umqtt.simple import MQTTClient
import utime, xtools, requests

xtools.connect_wifi_led()

temp = None
weather_key = '<api_key>'

ADAFRUIT_IO_USERNAME = "<username>"
ADAFRUIT_IO_KEY      = "<api_key>"
FEED = "keys"

com = UART(2, 9600, tx=17, rx=16)
com.init(9600)
# MQTT 客戶端
client = MQTTClient (
    client_id = xtools.get_id(),
    server = "io.adafruit.com",
    user = ADAFRUIT_IO_USERNAME,
    password = ADAFRUIT_IO_KEY,
    ssl = False,
)

def sub_cb(topic, msg):
    global temp
    city = None
    msg_str = msg.decode().strip().lower().replace(" ", "%20")
    print("收到訊息: ", msg_str)
  
    if msg_str == '1':
            city = 'Taipei'
            
    elif msg_str == '2':
            city = 'Taoyuan'
            
    elif msg_str == '3':
            city = 'New+Taipei'
            
    elif msg_str == '4':
            city = 'Taichung'
            
    elif msg_str == '5':
            city = 'Tainan'
            
    elif msg_str == '6':
            city = 'Kaohsiung'
    else:
        city = msg_str

    try:
        if city:
            url = f"https://api.openweathermap.org/data/2.5/weather?q={city}&units=metric&appid={weather_key}"
            response = requests.get(url)
            data = response.json()
            temp = data['main']['temp']
    
            print('傳送',temp)
            temp_str = f"{temp:05.2f}"
            com.write(temp_str.encode())  
            utime.sleep(2)
        else:
            print("Invalid city command")
    except:
        print("Error")
    
            
    

client.set_callback(sub_cb)   # 指定回撥函數來接收訊息
client.connect()              # 連線

topic = ADAFRUIT_IO_USERNAME + "/feeds/" +FEED
print(topic)
client.subscribe(topic)      # 訂閱主題




print('MicroPython Ready...')  # 輸出訊息到終端機

while True:
    client.check_msg()
    utime.sleep(1)

     

