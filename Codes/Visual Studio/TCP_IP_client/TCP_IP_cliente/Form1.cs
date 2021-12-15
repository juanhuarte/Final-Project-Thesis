using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.IO.Ports;
using System.Xml.Linq;
using System.Linq.Expressions;
using System.CodeDom;
using System.Collections;
using System.Net;
using System.Net.Sockets;
using System.Security.Cryptography;
using System.Threading;
using System.IO;

namespace TCP_IP_cliente
{
    public partial class Form1 : Form
    {

        // Variable del socket
        Socket sock;
       
       // Variable de lectur/escritura del socket
        NetworkStream stream = default(NetworkStream);

        // Creamos el archivo de texto para almacenar los bytes de las muestras
        StreamWriter escritor = new StreamWriter("testfile.txt", true);

         // Variables del grafico
        Graphics lienzo;
        Pen lapiz;
        int tActual = 0;
        int tAnterior = 0;
        int pAnterior = 0;
        int pActual = 0;
       
        int banderaArchivo = 0; // flag para indicar que se almacenen las muestras en el archivo

        // buffer de entrada (cola)
        Queue<int> colaRecepcion;

        public Form1()
        {
            InitializeComponent();
            
            colaRecepcion = new Queue<int>();

            //  Inicializacion del panel del grafico
            lienzo = panel1.CreateGraphics();
            lapiz = new Pen(Color.Black);
           
            // Hace las lecturas de la direccion IP y del puerto, indicada por los texbox de la ventada de aplicacion
            IPAddress ipAddress = IPAddress.Parse(txtHost.Text);
            IPEndPoint remoteEP = new IPEndPoint(ipAddress, Convert.ToInt32(txtPort.Text));
            
            
        }
       Socket socket()  // funcion para iniciar el soccket
        {
            return new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        }

        private void Iniciar_Click(object sender, EventArgs e)  // Respues al evento para iniciar la recepcion de las muestras
        {
            lapiz.Width = 1;
            colaRecepcion.Clear(); //limpia la cola
            
            try
             {
                sock = socket();
                sock.Connect(new IPEndPoint(IPAddress.Parse(txtHost.Text), Convert.ToInt32(txtPort.Text)));
                stream = new NetworkStream(sock); // Asignamos un stream al socket
            }
             catch
             {
                 MessageBox.Show("Error de conexion");
                stream.Close();
                sock.Close();
                
             }
            timer1.Enabled = true; // habilitamos el timer del programa
        }



        private void detener_Click(object sender, EventArgs e)  // evento para detener la recepcion de las muestras
        {
            timer1.Enabled = false;
            stream.Close();
            sock.Close();
            colaRecepcion.Clear();
        }

        private void timer1_Tick(object sender, EventArgs e) // evento para desencolar y graficar en la ventana de aplicacion
        {            
            int LSB = 0; // acumula los bytes bajos
            int MSB = 0; //acumula los bytes altos
            int cont = 1; // contador para promediar las muestras
            int factorEscala = 4096 / (350-10);
            int sincronizado = 0; // flag de sincronizacion
            int marca = 0; // flag de sincronizacion
            int aux; // lectura auxiliar para cuando hay que descartar una muestra
            int dato1 = 0; // byte LSB
            int dato2 = 0; // byte MSB


            try
            {
                if (stream.DataAvailable)  
                {
                    new Thread(() =>  
                    {
                        read();   // llamado a la funcion de recepcion
                    }).Start();
                }
                while (colaRecepcion.Count >= 2)   // verifica que hay 2 bytes disponibles en la cola
                {
                   // deseconla los bytes
                   dato1 = colaRecepcion.Dequeue();  
                   dato2 = colaRecepcion.Dequeue();
                    // verifica el sincronismo
                    if (dato1 == 255)
                    {
                        if (dato2 == 255)
                        {
                            sincronizado = 1;
                            marca = 0;
                        }
                        else if (marca == 1)
                        {
                            sincronizado = 1;
                        }
                    }
                    else if (dato1 != 255 && marca == 1)
                    {
                        marca = 0;
                        sincronizado = 0;
                    }
                    else if (dato2 == 255)
                    {
                        marca = 1;
                    }
                    if (sincronizado == 0 && marca == 0)
                    {
                        // acumula los bytes bajos y alto para promediar las muestras
                        LSB = LSB + dato1;
                        MSB = MSB + dato2;
                        cont++;
                        // guardar los bytes en el archivo
                        if (banderaArchivo == 1)
                        {
                            escritor.Write(dato1);
                            escritor.Write(",");
                            escritor.Write(dato2);
                            escritor.Write(",");
                        }

                    }
                    if (sincronizado == 1 && marca == 1)
                    {
                        marca = 0;
                        sincronizado = 0;
                        aux = colaRecepcion.Dequeue(); // lectura aux para descartar la muestra
                    }
                    else if (sincronizado == 1)
                    {
                        sincronizado = 0;
                      
                    }
                    if (cont == 20) // numero de muestras a promediar
                    {
                        pActual = (int)((float)-1 * ((MSB / cont * 256 + LSB / cont) / factorEscala - panel1.Size.Height+1)); // convertir las muestras a los valores comprendidos por el ADC
                      
                        LSB = 0;
                        MSB = 0;
                        cont = 1;
                        tActual = tAnterior + 1;
                        lienzo.DrawLine(lapiz, tAnterior, pAnterior, tActual, pActual); // puntos a graficar
                        pAnterior = pActual;
                        tAnterior = tActual;
                        if (tActual > 1000)
                        {
                            // vaciar la ventana grafica
                            tAnterior = 0;
                            tActual = 0;
                            lienzo.Clear(Color.AliceBlue);
                            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
                            this.panel1.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("panel1.BackgroundImage")));
                        }
                    }
                }
            }
            catch
            {
                timer1.Enabled = false;
                MessageBox.Show("Excepcion");
            }

        }

       void read() // funcion para recepcionar los datos en la cola
        {
            int cant_bytes = 200;  // 20kHz* 10ms (los 10ms son del timer)
            for (int i = 0; i < cant_bytes; i++)
            {
                colaRecepcion.Enqueue(stream.ReadByte());
            }  
        }

        private void button1_Click(object sender, EventArgs e) // evento para cambiar el flag para empezar a almacenar los bytes
        {
            escritor.WriteLine(" ");
            escritor.WriteLine("Datos nuevos: "); // linea que se utiliza para separar las muestras almacenadas en el archivo
            banderaArchivo = 1;
        }

        private void button2_Click(object sender, EventArgs e) // evento para dejar de almacenar las muestras
        {
            banderaArchivo = 0;
        }
    }
}
