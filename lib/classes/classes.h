/**
 * @file classes.h
 * @author EDUARDO JOSÉ DOS SANTOS (eduardo22santos@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef CLASSES_h
#define CLASSES_h
#include <DallasTemperature.h>
#include <Adafruit_HTU21DF.h>
#include <RTClib.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <SD.h>
#include <SPI.h>


/**
 * @brief Armazena as informações fornecidas por um arquivo json no cartão sd na 
 * memória do dispositivo
 * 
 */
struct Configuracao {
  //SSid do wifi
  char wifiSsid[64];
  //Senha do wifi
  char wifiSenha[64];
  //eduroan login
  char eduroanLogin[64];
  //eduroan senha
  char eduroanSenha[64];
  //Endereço do servidor NTP, responsável por sincronizar o horario
  char servidorNtp[64];
  //Define o fuso horário do Relógio
  int timeZone;
  //Define o intervalo em que o aparelho irá salvar os dados no cartão de memória
  //O tempo é medido em minutos
  float intervaloSalvar;
  //Indica se a conexão wifi é do tipo enterprise
  bool eduroamStatus;
  //Endereço de um servidor para testar conexão de internet, o padrão é usar o google.com
  char hostTest[64];
};

/**
 * @brief Armazena as informações de módulos e sensores, e também as informaçẽos de configuração do dispositivo
 * 
 */
struct Dados
{
    //Temperatura na extremidade do dispositivo
    float tempExtremidade;
    //Temperatura no centro do dispisitivo
    float tempCentro;
    //Umidade capturada pelo dispositivo
    float umidade;
    //Objeto as informaçẽos de horario obitidas pelo rtc
    DateTime horario;
    //Informações de configuração para uso interno do sistema
    Configuracao configuracao;
    DallasTemperature _ds18b20_extremidade, _ds18b20_centro;
    Adafruit_HTU21DF htu21d;
    RTC_DS3231 relogio;

    /**
     * @brief Construct a new Dados object
     * 
     * @param a sensor ds18b20 localizado na extremidade
     * @param b sensor sd18b20 localizado no centro
     */
    Dados(DallasTemperature a,DallasTemperature b);
    /**
     * @brief Atualiza a leitura dos sensores
     * 
     */
    void atualizar();
    /**
     * @brief Inicializa os sensores e módulos
     * 
     * @return true se todos os sensores e modulos iniciarem
     */
    boolean begin();
    /**
     * @brief Obtem as informações de configuração do cartão SD
     * 
     */
    void carregarConfiguracao();
    /**
     * @brief Exporta um arquivo de configuração para o cartão SD para facilitar o preenchimento dos dados
     * 
     */
    void carregarArquivoExemplo();
    /**
     * @brief salva os dados no cartão de memoria
     * 
     * @param file caminho paro o arquivo datalog
     */
    void salvarDadosSd(const char * file);
    /**
     * @brief emprime no serial um json com os dados mais recentes
     * 
     */
    void print();
    /**
     * @brief atualiza os dados de sensores, salva no sd, e emprime no serial
     * 
     * @param file caminho paro o arquivo datalog
     */
    void work(const char * file);

};

/**
 * @brief Cria um diretório
 * 
 * @param fs 
 * @param path é o nome e caminho do diretório no formato "/diretorio"
 */
void createDir(fs::FS &fs, const char * path);

/**
 * @brief Adiciona uma linha no arquivo contendo a informações que serão salvas no arquivo
 * 
 * @param fs 
 * @param path é o diretório do arquivo
 * @param message é a informação a ser adicionada no arquivo
 */void appendFile(fs::FS &fs, const char * path, const char * message);






#endif // CLASSES_h