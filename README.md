<h2>Bluetooth meat thermometer to MQTT</h2>

Who doesn't love the bluetooth meat thermometers? You can put it in your meat and monitor the temperature from your couch. However, there is one big downside and that you have to be in a certain range of the thermometer to be able to monitor it. Once you go inside the house or go to the store you cannot monitor it anymore.

So I thought, why not solve this problem by sending the temperature to a MQTT server that you can access from anywhere!

<h3>Necessities</h3>
<strong>Arduino board</strong>
For the arduino board I used a ESP32 devkit, but you can basically use any arduino board. Just make sure it has wifi and bluetooth capabilities.

<strong>Meat thermometer</strong>
Now this is the hard part about this guide..every thermometer is different and probably sends out their message in a different way. So to be clear: this is not a universal guide for every thermometer there is. However at the bottom of this guide I will explain how you can find and understand the bluetooth messages from your thermometer. 

I have a thermometer from the brand "Claire" which has 6 probes and uses the Grill 5.0 app from the app store. So this guide will definitely work for that brand.

<strong>MQTT Server</strong>
A MQTT server is of course a requirement. You can install this on your server, on a NAS or just on a computer. You can download MQTT from <a href="https://www.mosquitto.org/download/">here</a>. For troubleshooting or testing you can use <a href="http://mqtt-explorer.com/">MQTT Explorer</a>

<strong>Home Assistant (optional)</strong>
I use home assistant for all my home automation. I made sure it is available from outside the house too so for me this was the most logical choice. 

<strong>Power bank (optional)</strong>
I use mine mostly for the BBQ, which is outside and power outlets are not easily available. So I use a power bank to power my arduino


<h3>Installation</h3>
Open your Arduino IDE and make sure the following libraries are installed:
<ul>
    <li>ArduinoBLE</li>
    <li>Wifi101</li>
    <li>ArduinoMqttClient</li>
</ul>

Download the bt_temp_to_mqtt file from this repository and load that in your IDE. Make sure you edit lines 8 through 12 with the values for your environment. You can also edit lines 15 through 21 if you want to edit the MQTT topics.

Upload the script to your Arduino and you're done..easy as that! Make sure the Arduino is inside the bluetooth range of your thermometer. 



<h3>Guide to finding and understanding your bluetooth signal</h3>
The hardest part of this project was to find and understand the bluetooth signals. What helped me quite a lot was to install an app on my phone "nRF Connect" (available for both iPhone and Android). It will scan your surroundings for bluetooth devices, will display them and you can also connect to them and see all the information the device sends.

Each bluetooth device advertises a name so that part was pretty easy. In my case the device had the name "Grill BT5.0" (line 25 of the code). 

Now the hard part was to find the characteristic to which the temperature is being send. Characteristics don't have logical names like "tempvalues" or something so you have to go through each one of them and find the correct one. Also pay attention that the values might not be integers. You're probably looking for an integer value that represents the temperature, however in my case the value for all the probes were stored in a byte array. (see lines 155 through 164)

If you've found the right characteristic the rest is quite easy. Make sure you retrieve it in your code and that you send it to a MQTT server.