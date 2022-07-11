/**
 * @file main.cpp
 * @author Eduardo José dos Santos (eduardo22santos@hotmail.com)
 * @brief Sistema embarcado para dispositivo de monitoramento de silos agrícolas
 * @version 0.1
 * @date 2022-07-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <Arduino.h>
#include <DallasTemperature.h>
#include <Adafruit_HTU21DF.h>
#include <RTClib.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <SD.h>
#include <SPI.h>
#include <classes.h> //BIBLIOTECA ORIGINAL DO PROJETO

/**
 * @brief fator de conversão de microsegundos para segundos
 * 
 */
#define uS_TO_S_FACTOR 1000000


/**********************************************************
 * CONFIGURANDO OS SENSORES DS18B20
 * ********************************************************
 **/
// DEFININDO OS PINOS DOS SENSORES DS18B20
#define sensor_bico 12
#define sensor_centro 14
//EXTANCIANDO OS SENSORES
OneWire sensor1(sensor_bico);
OneWire sensor2(sensor_centro);
DallasTemperature extremidade(&sensor1);
DallasTemperature centro(&sensor2);
/**
 * @brief OBJETO QUE CONTEM OS DADOS E OS MÉTODOS NECESSÁRIOS AS LEITURAS.
 * OS SENSORES E MÓDULOS HTU21D E RTC3231 JÁ ESTÃO ESTANCIADOS E SÃO INICIALIZADOS
 * NO MÉTODO begin()
 * 
 * @return Dados 
 */
Dados dados(extremidade,centro);

/**
 * @brief 
 * 
 */
void setup() {
    //iniciando Serial usb
    Serial.begin(9600);
    //Iniciando os sensores e módulos
    while (!dados.begin())
    {
        delay(1000);
    }
    //Iniciando a conexão com o cartão de memória
    while (!SD.begin())
    {
        Serial.println("FALHA AO ACESSAR CARTÃO SD!");
        delay(1000);
    }
    //Procurando pelo o arquivo que contem a configuração basica
    if (!SD.exists("/config.json"))
    {
        //Criando um arquivo de exemplo no cartão de memória, caso não haja um arquivo de configuração
        dados.carregarArquivoExemplo();
        Serial1.println("Ensira os dados de configuração!");        
    }else
    {
        //Carregando o arquivo de configuração para a memoria do CI
        dados.carregarConfiguracao();
    }
    //Verifica se existe o arquivo datalog no cartao de memoria, criando-o caso contrario
    if (!SD.exists("/datalog.csv")) //verifica ou cria o arquivo datalog.txt
    {
        File file = SD.open("/datalog.csv", FILE_WRITE);
        file.close();
        appendFile(SD, "/datalog.csv", "data,horario,sensor1,sensor2,media,umidade");   
    }

    //Finalizando a conexão com o cartão de memória
    SD.end();
    
}
/**
 * @brief 
 * 
 */
void loop() {   
    //faz a leitura dos sensores, salva no sd, emprime no serial
    dados.work("/datalog.csv");
    //hiberna o CI no tempo determinado no arquivo de configuração
    esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * dados.configuracao.intervaloSalvar);
    esp_deep_sleep_start(); //força o ESP32 entrar em modo SLEEP por um segundo
}