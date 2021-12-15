# Final-Proyect-Thesis
 This project describes the design of a portable low-power signal acquisition and wireless transmitter device with autonomous power. Its function is to acquire an analog signal with a maximum bandwidth of 10KHz and two range amplitudes of ± 10V and ± 100mV. This must be done keeping a minimum dynamic range of 60dB for both cases. Then the digitized signal has to be transmitted wirelessly to an external receiver where the samples are saved and eventually, its visualization is allowed. 
 
This work consists in the design of the device, starting from the analog stage of acquisition, adaptation and conditioning, following with the stage of digital conversion and processing, and ending with the transmission and reception. Additionally, was designed the device's own power supply adapted to its portable characteristics. 

Due to the Aislamiento Social Preventivo y Obligatorio1 (ASPO) forced in our country as a result of the COVID-19 pandemic, this work was carried out remotely and without physical access to the laboratory. For this reason, assembly, testing and commissioning of the device were not carried out. The results of both the analog and the power supply block were obtained through simulation software, as well as it was initially used for the digital block, while the real transmission was carried out via TCP / IP using the ESP32 development board and performing the reception on a computer.


## The following image represent the main purpose of this project 
![image](https://user-images.githubusercontent.com/66688256/146218986-29852e44-9781-412b-a9a5-fe896810a12a.png)

## Design

To carry out the project, once the transmission technology was established, a microcontroller capable of transmitting wirelessly was selected. It was decided to use an OLED screen as an interface for the user who is operating the device. Finally, the programming environment had to be chosen for both the transmitter and the receiver. The transmitter being the wireless acquirer, while the receiver is a computer.

## Frameworks


On the transmitter side, the software used was Arduino IDE, since it has a wide variety of information available on the internet. While, for the receiver side, two different programs were used, which made it possible to generate different user interfaces. On the one hand, Matlab was used to perform quick and simple tests. The second software is Visual Studio, which allows to generate application windows, this being a more professional tool allowing the user to work more safely.

Matlab It is a numerical computing system that offers an integrated development environment with its own programming language, called the M language. It allowed the connection to be made via TCP / IP. A simple code was designed that throughout the project helped us correct various problems that arose. For example, the synchronism between the two devices, check the transmission rate and the sampling time of the input signal.

For the design of the application window, a project is developed in Windows FormApp.

The different tools that were selected to generate the interface are the following ones. 
Four buttons, graphic panel, two “labels”, two text boxes and the timer. The role of each is detailed below.

1. Buttons:

• Start: makes the connection to the server and starts receiving data.
• Stop: disconnection occurs.
• Save samples: stores the low and high byte of each sample, below the indication “New data”.
• Finish saving: stops saving the bytes.


2. Graphic Panel:
• It allows observing the input signal in real time, previously an average of the input samples is carried out.


3. Label:
• They indicate the meaning of the text boxes.


4. Text boxes:
• Displays the IP address of the server.
• Shows the port used by the server.


5. Timer:
• Perform program interruption every specified time. Within it, the bytes coming from the server are read and they are also plotted in real time.

## Reception in Matlab

The initial test performed was to generate a sawtooth on the server. Where a counter was increased one by one until reaching the value of 4096, and then returning it to 0. This allows us to observe that there are no losses in the transmission. On the other hand, when sending a known value, errors can be detected, such as the loss of synchronism that was corrected.

![image](https://user-images.githubusercontent.com/66688256/146222452-e80af6b6-0e58-464c-a9fc-320947e6ee43.png)

## Reception in Visual Studio

In the case of Visual Studio, the same tests were carried out, the results obtained can be observed.

![image](https://user-images.githubusercontent.com/66688256/146222565-84e805d2-0bdd-45bb-90d3-b0e869ef1795.png)

## OLED Display 

The functionality of the OLED screen is to provide information to the user about what is happening in the device to facilitate its use.
The user will be able to visualize the battery level of the device, for this a function was created in the Arduino IDE so that this value is constantly updated, since it is of the utmost importance for the customer. As a first test, the battery level is given by the value given by a potentiometer. To do this, it was connected to a channel of the ADC and values between 0 and 4095 are obtained, where 0 corresponds to 0% and 4095 to 100%.
When the device is turned on, the Arduino IDE Setup is executed where it indicates to the user through the screen that the device was turned on and then visualizes that it is connecting to the Wi-Fi network.

![image](https://user-images.githubusercontent.com/66688256/146223353-9c8d119f-f53f-4840-a36c-d1e472fdfebb.png)
![image](https://user-images.githubusercontent.com/66688256/146223375-01c964e7-a422-429a-abde-c5b10e18811c.png)
![image](https://user-images.githubusercontent.com/66688256/146223382-5592030a-6207-4c03-83d2-f4f0ca33c9b8.png)


Once the Setup is finished, it is verified if the TCP / IP connection between the server and the client was made successfully. If so, if the client is connected it will appear on the screen as “client connected”, otherwise it will say “client disconnected”.

![image](https://user-images.githubusercontent.com/66688256/146223449-b6b834d5-161f-40b5-875c-b274c4827adf.png)
![image](https://user-images.githubusercontent.com/66688256/146223469-d62899e2-b49d-4bca-9bd9-37563ce22840.png)

## To conclude

Finally we desing printed circuit board as shown in the following images:

![image](https://user-images.githubusercontent.com/66688256/146223857-f17bafe6-7b40-4212-b545-5f4f4bce579b.png)
![image](https://user-images.githubusercontent.com/66688256/146223883-b3511469-684b-4a5a-abb1-f45d3e77425c.png)


