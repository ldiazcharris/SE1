# Unidad 2 - Programación básica de Microcontroladores.

## Índice

- [2.1. Uso del periférico UART.](#21-uso-del-periférico-uart)
- [2.2. Integración de sensores analógicos.](#22-integración-de-sensores-analógicos)
- [2.3. Integración de sensores digitales.](#23-integración-de-sensores-digitales)
- [2.4. Manejo del tiempo.](#24-manejo-del-tiempo)
- [2.5. Integración con visualizadores.](#25-integración-con-visualizadores)
- [2.6. Integración con actuadores de control On-Off.](#26-integración-con-actuadores-de-control-on-off)
- [2.7. Integración con actuadores de control continuo.](#27-integración-con-actuadores-de-control-continuo)


## 2.1. Uso del periférico UART

Las siglas UART traducen: Universal Asynchronous Receiver/Transmitter (Transmisor/Receptor Asíncrono Universal). Este es un protocolo de comunicación serie entre dos dispositivos. Un ejemplo del uso de este protocolo es en la comunicación con interfaz RS-232. El protocolo UART utiliza dos líneas entre el transmisor y receptor para transmitir y recibir en ambas direcciones. La conexión se realiza de forma cruzada. Es decir, la línea de transmisión del primer dispositivo se conecta a la línea de transmisión del segundo y viceversa. La comunicación en el UART puede ser simplex, semidúplex, o full-dúplex. A continuación, se presenta un diagrama de conexión UART entre dos dispositivos:

<img src="imagenes/2.1_Uart_Block_diagram.png" width=500>

*Figura 1. Conexión UART.*

### Es serie

Los datos en el protocolo UART se transmiten uno tras otro en el mismo canal (por eso es serie) en forma de tramas de bits. El formato y el contenido de estas tramas se describe y explica brevemente.

<img src="imagenes/2.1_Trama_UART.png" width=500>

*Figura 2. Formato de Trama UART.*

El protocolo UART no define voltajes o rangos de voltaje específicos para los niveles lógicos de transmisión 0 o 1 (bajo o alto), esto va a depender específicamente de los fabricantes de chips. Por lo tanto, algunos autores llaman al nivel a alto como “marca” mientras que al nivel bajo lo llaman “espacio”. Mientras no se están trasmitiendo datos, la línea de transmisión se mantiene en estado alto, este estado también se le conoce como “estado Idle”. 

### Bits de start y stop

Ya que el UART es asíncrono, el transmisor necesita indicar que los bits de datos están llegando. Esto se logra utilizando el bit de inicio. El bit de inicio es una transición del estado alto de reposo a un estado bajo, y seguido inmediatamente por bits de datos de usuario.

Después de que se terminan los bits de datos, el bit de parada indica el fin de datos de usuario. El bit de parada es una transición de regreso al estado alto o Idle o permanece en el estado alto por un tiempo de bit adicional. Se puede configurar un segundo bit de parada (opcional), generalmente para darle tiempo al receptor de prepararse para la siguiente trama, pero esto no es común en la práctica.

### Bits de datos 

Los bits de datos son los datos de usuario o bits “útiles” y vienen inmediatamente después del bit de inicio. Puede haber de 5 a 9 bits de datos de usuario, aunque de 7 o 8 bits es lo más común. Estos bits de datos normalmente se transmiten primero con el bit menos importante.


### Bit de paridad

Una trama UART puede también contener un bit de paridad opcional que puede usarse para la detección de errores. Este bit se inserta entre el final de los bits de datos y el bit de parada. El valor del bit de paridad depende del tipo de paridad que esta siendo usado (par o impar):
En la paridad par, este bit se configura para que el total de números 1 en la trama será par.
En la paridad impar, este bit se configura para que el total de números 1 en la trama será impar

### Es asíncrono

El protocolo UART es asíncrono. Es decir, el transmisor y el receptor no comparten una señal de reloj. Esto ayuda a simplificar el protocolo, sin embargo, por esta misma razón el transmisor y el receptor deben “comprometerse” a transmitir a la misma velocidad, tener el mismo tamaño de trama y tener las mismas reglas de paridad para que la comunicación sea. Las velocidades de baudios del UART más comunes que se utilizan en estos días son 4800, 9600, 19.2K, 57.6K y 115.2K. 

### [Práctica 2.1. UART](2.1_practica_2_1_UART.md)

## 2.2. Integración de sensores analógicos

### Procesamiento digital de señales

El procesamiento digital de la señal convierte señales de naturaleza analógica, tales como el sonido, el vídeo e información procedente de sensores, en formato digital, utilizando técnicas digitales para mejorar y modificar los datos de las señales analógicas para distintas aplicaciones [[2]](#referencias).

Debido a que los dispositivos basados en tecnología digital como lo son los Microcontroladores, no tienen compatibilidad directa con electrónica analógica, es necesaria la implementación de circuitos intermedios que permitan acoplar los distintos niveles de tención y señales continuas con las discretas. 

**Ejemplo:**

<img src="imagenes/2.2_ADC_1.png" width=500>

*Figura 3. Diagrama de bloques de un sistema digital para control de temperatura [Tomado de: [1]](#referencias).*

Un sistema de procesamiento digital de señal tiene las siguietnes etapas básicas: 

- Traduce primero una señal analógica que varía de manera continua a una serie de niveles discretos. Donde se modifica la señal analógica para obtener una aproximación de la misma en formato de "escalera". Este proceso se realiza mediante un circuito de muestreo y retención [[2]](#referencias).

	<img src="imagenes/2.2_escalera_señal_senoidal.png" width=500>

	*Figura 4. Señal senoidal con aproximación de escalera [Tomado de: [2]](#referencias).*

- Luego viene el proceso de cuantificación, que busca obtener una serie de códigos binarios que representan cada uno de los pasos discretos de esa aproximación, mediante un proceso denominado conversión analógico-digital (A/D). El circuito que realiza la conversión A/D se denomina convertidor analógico-digital (ADC, Analog-to-Digital Converter) [[2]](#referencias).

	<img src="imagenes/2.2_cuantificacion.png" width=500>

	*Figura 5. Muestreo, retención y cuantificación de una onda senoidal [Tomado de: [2]](#referencias).*

- Luego de la conversión analógico-digital, se la aplica a un procesador digital de la señal (DSP, Digital Signal Proccesor). El DSP puede realizar diversas operaciones con los datos entrantes, como por ejemplo eliminar las interferencias no deseadas, aumentar la amplitud de ciertas frecuencias de la señal y reducir la de otras, codificar los datos para realizar una transmisión segura de los mismos y detectar y corregir errores en los códigos transmitidos [[2]](#referencias). 

- Luego de este procesamiento digital, la señal debe convertirse nuevamente en analógica. Este paso se lleva a cabo mediante un convertidor digital-analógico (DAC, Digital-to-Analog Converter). 


<img src="imagenes/2.2_bloques_proc_digital_señales.png" width=500>

*Figura 6. Diagrama de bloques de un sistema básico de procesamiento digital de señales [Tomado de: [2]](#referencias).*

### Conversión analógica-digital

Es el proceso por el que una magnitud analógica se convierte a formato digital. Los circuitos electrónicos que realizan esta labor, son llamados *Convertidores analógicos a digital (ADC, del inglés analog-to-digital converter)*. 

Existen dos parámetros importantes para los ADC:

- Resolución: el número de bits de la conversión.  
- Tasa de transferencia: la frecuencia de muestreo que un ADC puede aceptar, en número de muestras por segundo.

Existen diferentes tipos de circuitos capaces de realizar la conversión analógica-digital:
- Convertidor analógico-digital por aproximaciones sucesivas *(SAR, Successive Approximation Register)*.
- Convertidor analógico-digital sigma-delta (S-D).
- Convertidor analógico-digital flash
- Convertidor analógico-digital de pendiente doble

Si desea saber más a cerca de estos convertidores, puede consultar el libro de Thomas L. Floyd. FUNDAMENTOS DE SISTEMAS DIGITALES [[2]](#referencias), en la sección *13.3 MÉTODOS DE CONVERSIÓN ANALÓGICA-DIGITAL*.

### ADC en el ESP32

El chip ESP32 integra dos convertidores analógico-digital por aproximaciones sucesivas *(SAR)* de 12 bits (2^12 valores enteros, de 0 a 4095) y admite dos canales ADC [[3]](#referencias):

- ADC1, con 8 pines, conectados a los GPIO 32 - 39 [[3]](#referencias).
- ADC2, con 10 pines, conectados a los GPIO 0, 2, 4, 12 - 15 y 25 - 27, con las siguientes restricciones para crear aplicaciones [[3]](#referencias):
	- El ADC2 es utilizado por el controlador Wi-Fi. Por lo tanto, la aplicación solo puede usar ADC2 cuando el controlador Wi-Fi no se ha iniciado [[3]](#referencias).
	- Algunos de los pines ADC2 se usan como (strapping pins) (GPIO 0, 2, 15), por lo tanto, no se pueden usar libremente. Tal es el caso en los siguientes Kits oficiales de desarrollo:
		- ESP32 DevKitC: GPIO 0 no se puede usar debido a circuitos externos de programas automáticos [[3]](#referencias).
		- ESP-WROVER-KIT: GPIO 0, 2, 4 y 15 no se pueden usar debido a conexiones externas para diferentes propósitos [[3]](#referencias).

### Configuración y lectura ADC en el ESP32 con ESO-IDF

Cada unidad ADC admite dos modos de trabajo, ADC-RTC o modo ADC-DMA. 

- ADC-RTC muestreo de baja frecuencia [[3]](#referencias).
- ADC-DMA muestreo continuo de alta frecuencia [[3]](#referencias).

El ADC debe configurarse antes de tomar la lectura. Para hacer uso de las funciones del ADC con el framwork ESP-IDF, es necesario incluir la biblioteca 

~~~
#include "driver/adc.h"
~~~ 

**Para ADC1**, configure la precisión y atenuación deseadas llamando a las funciones [[3]](#referencias)

~~~
adc1_config_width([resolución en bits])
~~~ 

**Opciones de resolución en el ESP32-WROOM**
| ---------------  | -----------------------  |
| ---------------  | -----------------------  |
| ADC_WIDTH_BIT_9  | DC capture width is 9Bit. |
| ADC_WIDTH_BIT_10 | ADC capture width is 10Bit. |
| ADC_WIDTH_BIT_11 | ADC capture width is 11Bit. |
| ADC_WIDTH_BIT_12 | ADC capture width is 12Bit. |

~~~
adc1_config_channel_atten([canal ADC], [Atenuación])
~~~

**Canales del ADC1**

| Macro de la biblioteca  | Canal ADC1     | GPIO Correspondiente | 
| ----------------------- | -------------- | -------------------- |
| ADC1_CHANNEL_0          | ADC1 channel 0 | GPIO36               |
| ADC1_CHANNEL_1          | ADC1 channel 1 | GPIO37               |
| ADC1_CHANNEL_2          | ADC1 channel 2 | GPIO38               |
| ADC1_CHANNEL_3          | ADC1 channel 3 | GPIO39               |
| ADC1_CHANNEL_4          | ADC1 channel 4 | GPIO32               | 
| ADC1_CHANNEL_5          | ADC1 channel 5 | GPIO33               |
| ADC1_CHANNEL_6          | ADC1 channel 6 | GPIO34               | 
| ADC1_CHANNEL_7          | ADC1 channel 7 | GPIO35               |

**Opciones de atenuación en el ESP32-WROOM**


|          | Atenuación  | Rango sugerido  |
|    SoC   |     (dB)    |      (mV)       |
| -------- | ----------- | --------------- |
|   ESP32  |       0     |    100 ~  950   |
|   ESP32  |       2.5   |    100 ~ 1250   |
|   ESP32  |       6     |    150 ~ 1750   |
|   ESP32  |      11     |    150 ~ 2450   |
| -------- | ----------- | --------------- |
| ESP32-S2 |       0     |      0 ~  750   |
| ESP32-S2 |       2.5   |      0 ~ 1050   |
| ESP32-S2 |       6     |      0 ~ 1300   |
| ESP32-S2 |      11     |      0 ~ 2500   |


**Para ADC2**, configure la atenuación por [[3]](#referencias)
~~~
adc2_config_channel_atten([Canal ADC2], Atenuación)
~~~

| Macro de la biblioteca  | Canal ADC1     | GPIO ESP32 | GPIO ESP32-S2 | 
| ----------------------- | -------------- | ---------- | ------------- |
| ADC2_CHANNEL_0          | ADC2 channel 0 | GPIO4      | GPIO11        |
| ADC2_CHANNEL_1          | ADC2 channel 1 | GPIO0      | GPIO12        |
| ADC2_CHANNEL_2          | ADC2 channel 2 | GPIO2      | GPIO13        |
| ADC2_CHANNEL_3          | ADC2 channel 3 | GPIO15     | GPIO14        |
| ADC2_CHANNEL_4          | ADC2 channel 4 | GPIO13     | GPIO15        |
| ADC2_CHANNEL_5          | ADC2 channel 5 | GPIO12     | GPIO16        |
| ADC2_CHANNEL_6          | ADC2 channel 6 | GPIO14     | GPIO17        |
| ADC2_CHANNEL_7          | ADC2 channel 7 | GPIO27     | GPIO18        |
| ADC2_CHANNEL_8          | ADC2 channel 8 | GPIO25     | GPIO19        |
| ADC2_CHANNEL_9          | ADC2 channel 9 | GPIO26     | GPIO20        |

El ancho de lectura de ADC2 se configura cada vez que toma la lectura [[3]](#referencias).

La configuración de atenuación se realiza por canal, según los tipos de dato [[3]](#referencias): `adc1_channel_t` y `adc2_channel_t` 


Para leer el resultado de conversión de ADC se usa [[3]](#referencias):

Para el ADC1
~~~
adc1_get_raw([Canal ADC1])
~~~
y Para el ADC2, 
~~~
adc2_get_raw([Canal ADC2], [Resolución en bits], [puntero a variable])
~~~

El ancho de lectura de ADC2 debe establecerse como un parámetro de la función  ~adc2_get_raw()~ en lugar de en las funciones de configuración [[3]](#referencias).

### [Práctica 2.2. Integración de sensores analógicos](2.2_practica_2_2_Sensores_Analogicos.md)

## 2.3. Integración de sensores digitales

## 2.4. Manejo del tiempo

## 2.5. Integración con visualizadores

## 2.6. Integración con actuadores de control On-Off

## 2.7. Integración con actuadores de control continuo

## Referencias

- [1] TOCCI, RONALD J., NEAL S. WIDMER, GREGORY L. MOSS. Sistemas digitales. Principios y aplicaciones. Décima edición. Pearson Educación, México, 2007. ISBN: 978-970-26-0970-4. Área: Ingeniería. 

- [2] Thomas L. Floyd. FUNDAMENTOS DE SISTEMAS DIGITALES. PEARSON EDUCACIÓN S.A., Madrid, 2006. ISBN 10: 84-8322-085-7. ISBN 13: 978-84-832-2720-6.
	13.1 FUNDAMENTOS DEL PROCESAMIENTO DIGITAL DE LA SEÑAL Pág. 854
	Diagrama de bloques básico de un sistema típico de procesamiento digital de la señal Pág. 855
	Conversión analógico-digital Pág. 859
	13.3 MÉTODOS DE CONVERSIÓN ANALÓGICA-DIGITAL Pág. 862 

- [3] https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/adc.html

- [4] https://github.com/espressif/esp-idf/tree/v4.2/examples/peripherals/adc

- [5] https://linuxhint.com/esp32-adc-read-analog-values-with-arduino-ide/

- [6] https://www.electronicshub.org/esp32-adc-tutorial/

- [9] SAR Animación. https://en.wikipedia.org/wiki/Successive-approximation_ADC#/media/File:4-bit_Successive_Approximation_DAC.gif
