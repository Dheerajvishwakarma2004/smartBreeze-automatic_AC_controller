# smartBreeze-Automatic AC Control System Using ESP8266

A smart and cost-effective IoT-based AC control system that uses ESP8266, DHT11, PIR motion sensor, and IR transmitter to automate air conditioning based on temperature, motion, and user commands via a Flutter mobile app.

## 🚀 Features

- 🧠 **Smart Automation**: Turns AC on/off based on room occupancy and temperature thresholds.
- 🌡️ **Real-time Temperature Monitoring**: Uses DHT11 sensor.
- 👀 **Motion Detection**: Detects human presence using a PIR sensor.
- 📱 **Mobile App Control**: Flutter-based application for manual control.
- 🎙️ **Voice Commands**: Supports voice-based control for hands-free usage.
- 🌐 **Web Interface**: Access and control via local IP through web browser.
- 🔌 **Low Power & Cost-Efficient**: Based on ESP8266 for affordable Wi-Fi capability.

## 📷 Project Demo

![Hardware Setup](https://github.com/Dheerajvishwakarma2004/smartBreeze-automatic_AC_controller/blob/main/assets/hardware.jpg)
![App UI](https://github.com/Dheerajvishwakarma2004/smartBreeze-automatic_AC_controller/blob/main/assets/Flutter%20Ui%20.jpg)



## 🔌 Components Used

| Component        | Description                            | Approx. Cost (₹) |
|------------------|----------------------------------------|------------------|
| ESP8266          | Wi-Fi enabled microcontroller          | ₹400             |
| DHT11            | Temperature & Humidity Sensor          | ₹160             |
| PIR Sensor       | Motion Detection Sensor                | ₹240             |
| IR Transmitter   | Sends IR commands to AC                | ₹80              |
| Miscellaneous    | Resistors, Breadboard, Wires, etc.     | ₹400             |
| **Total Cost**   |                                        | **₹1280**        |

## ⚙️ Installation & Setup

1. **Hardware Setup**
   - Connect DHT11 and PIR sensors to ESP8266 as per the circuit diagram.
   - Connect IR transmitter to appropriate GPIO pin.
   - Power the board using USB or 5V supply.

2. **Software Setup**
   - Install [Arduino IDE](https://www.arduino.cc/en/software)
   - Install required libraries: `ESP8266WiFi`, `DHT`, `IRremoteESP8266`
   - Upload the provided code to ESP8266 via USB.

3. **Flutter App**
   - Clone the Flutter app from the `/flutter_app` folder.
   - Update IP address of ESP8266 server.
   - Run using: `flutter run`


## 📊 Results

| Condition                       | System Response            |
|--------------------------------|----------------------------|
| Motion detected + High temp    | AC Turns ON                |
| No motion                      | AC Turns OFF after delay   |
| Manual ON via App              | AC Turns ON                |
| Voice Command "Turn AC on"     | AC Turns ON                |

## 📈 Time-to-Market Strategy

- **Development Time**: 3 months  
- **Testing & Iteration**: 1 month  
- **Production Scaling**: 2 months  
- **Marketing & Launch**: 2 months  
- **Total Time to Market**: **8 months**

### Market Entry Delay Analysis

- Similar products (e.g., Sensibo, Ambi Climate) are already available in India.
- Delay of each month may lead to a projected revenue loss of **₹40,000**.
- Early beta testing, partnerships, and pre-launch marketing are essential to minimize loss.

## 📱 Flutter App Preview

> ![App UI](https://github.com/Dheerajvishwakarma2004/smartBreeze-automatic_AC_controller/blob/main/assets/Flutter%20Ui%20.jpg)

## 📜 License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## 🤝 Contributing

Contributions, ideas, and suggestions are welcome! Feel free to open issues or pull requests.

## 🙋‍♂️ Author

**Dheeraj Vishwakarma**  
[GitHub](https://github.com/Dheerajvishwakarma2004) • [LinkedIn](https://www.linkedin.com/in/dheeraj-vishwakarma-907264288)

**Sourajeet Sahoo**  
[GitHub](https://github.com/SourajeetOfficial) • [LinkedIn](https://www.linkedin.com/in/sourajeet-sahoo-29743025b)

**Yash Tiwari**  
[GitHub](https://github.com/yashtiwari0069) • [LinkedIn](https://www.linkedin.com/in/yash-tiwari-636b41284)

---





