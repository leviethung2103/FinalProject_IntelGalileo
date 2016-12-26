#!/usr/bin/python
import time, mraa
from ubidots import ApiClient
from decimal import Decimal
#Connect to Ubidots
for i in range(0,5):
    try:
        print "Requesting Ubidots token"
        api = ApiClient('244f8055946ac3df444da7bb8121a6a4f4151f16')
        break
    except:
        print "No internet connection, retrying..."
        time.sleep(5)
# Assign analog pins
a0 = mraa.Aio(0)
temperature = a0.read()*500/1023
# a1 = mraa.Aio(1)
#Send data to Ubidots inside an infinite loop
while(1):
	print(temperature)
	api.save_collection([
		{
			'variable' : '585c007076254266dcf27744',
			'value' : temperature,
			'context': {
			'lat' :	16.053266,
			'lng':  108.214324
			#BachKhoa
	#		'lat':  ,
	#		'lng': 
			} 
		}
		])
    # api.save_collection([{'variable': '585c007076254266dcf27744','value':a0.read()}])
    # print(a0.read())

#Read analog pins and send values to Ubidots

# while(1):
# 	try:
# 	  api.save_collection([{'variable':
# 	  	'Put_Here_Your_Variable_ID_1','value':a0.read()}, {'variable':
# 	  	'Put_Here_Your_Variable_ID_2','value':a1.read()}])
# 	except:
# 		print("Couldn't send data.")
# 		continue

	#Update several variables in a single request
