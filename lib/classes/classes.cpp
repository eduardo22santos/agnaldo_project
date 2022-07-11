/**
 * @file classes.cpp
 * @author EDUARDO JOSÉ DOS SANTOS (eduardo22santo@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <classes.h>
/**
 * @brief Cria um diretório
 * 
 * @param fs 
 * @param path é o nome e caminho do diretório no formato "/diretorio"
 */
void createDir(fs::FS &fs, const char * path){
    fs.mkdir(path);
}

/**
 * @brief Adiciona uma linha no arquivo contendo a informações que serão salvas no arquivo
 * 
 * @param fs 
 * @param path é o diretório do arquivo
 * @param message é a informação a ser adicionada no arquivo
 */
void appendFile(fs::FS &fs, const char * path, const char * message){
    File file = fs.open(path, FILE_APPEND);
    if(file.println(message)){
    } else {
        SD.end();
        delay(500);
        SD.begin();
    }
    file.close();
}

void Dados::atualizar(){
    _ds18b20_extremidade.requestTemperatures();
    _ds18b20_centro.requestTemperatures();
    delay(750);
    tempExtremidade = _ds18b20_extremidade.getTempCByIndex(0);
    tempCentro = _ds18b20_centro.getTempCByIndex(0);
    umidade = htu21d.readHumidity();
    horario = relogio.now();

}
Dados::Dados(DallasTemperature a, DallasTemperature b){
    _ds18b20_extremidade = a;
    _ds18b20_centro = b;
}
boolean Dados::begin(){
    if (!relogio.begin())
    {
        Serial.println("Rtc DS3231 FALHOU!");
        return false;
    }
    _ds18b20_centro.begin();
    _ds18b20_extremidade.begin();
    if (!htu21d.begin())
    {
        Serial1.print("HTU21D FALHOU!");
        return false;
    }
    return true;  
}
void Dados::carregarConfiguracao(){
     // Open file for reading
    File file = SD.open("/config.json", "r");
    // Allocate a temporary JsonDocument
    // Don't forget to change the capacity to match your requirements.
    // Use arduinojson.org/v6/assistant to compute the capacity.
    StaticJsonDocument<1536> doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        Serial.println("Falha ao ler o arquivo de configuracaouração!");
    }else
    {
        strlcpy(configuracao.wifiSsid, doc["wifi_ssid"], sizeof(configuracao.wifiSsid));
        strlcpy(configuracao.wifiSenha, doc["wifi_senha"], sizeof(configuracao.wifiSenha));
        strlcpy(configuracao.eduroanLogin, doc["eduroam_login"], sizeof(configuracao.eduroanLogin));
        strlcpy(configuracao.eduroanSenha, doc["eduroam_senha"], sizeof(configuracao.eduroanSenha));
        configuracao.timeZone = doc["fuso_horario"];
        configuracao.intervaloSalvar = doc["intervalo_sd"];
        configuracao.eduroamStatus = doc["eduroam_status"];
        strlcpy(configuracao.servidorNtp, doc["ntp_host"], sizeof(configuracao.servidorNtp));
        strlcpy(configuracao.hostTest, doc["host_teste_conexao"], sizeof(configuracao.hostTest));
    }
    file.close();
}
void Dados::carregarArquivoExemplo(){
    File file = SD.open("/config.json", FILE_WRITE);

    // Allocate a temporary JsonDocument
    // Don't forget to change the capacity to match your requirements.
    // Use arduinojson.org/assistant to compute the capacity.
    StaticJsonDocument<768> doc;

    // Set the values in the document
    doc["wifi_ssid"] = "EDUARDO";
    doc["wifi_senha"] = "16r04o18b";
    doc["eduroam_login"] = "login da rede eduroam";
    doc["eduroam_senha"] = "senha da rede eduroam";
    doc["eduroam_status"] = false;
    doc["intervalo_sd"] = 60;
    doc["fuso_horario"] = -3;
    doc["ntp_host"] = "a.st1.ntp.br";
    doc["host_teste_conexao"] = "https://www.google.com/";

    serializeJsonPretty(doc, file);

    file.close();
}
void Dados::salvarDadosSd(const char * file){

    if (!SD.begin())
    {
        ESP.restart();
    }
    
    char horarioArquivo[9] = "hh:mm:ss";
    char dataArquivo[11] = "DD/MM/YYYY";
    //data,horario,sensor1,sensor2,media,umidade
    String datalog = String(horario.toString(dataArquivo)) + "," + 
        String(horario.toString(horarioArquivo)) + "," +
        String(tempExtremidade) + "," + 
        String(tempCentro) + "," + 
        String((tempExtremidade + tempCentro)/2) + "," + 
        String(umidade);

    //Salva no arquivo datalog
    appendFile(SD, file, datalog.c_str());
    SD.end();
}
void Dados::print(){

    char horarioArquivo[9] = "hh:mm:ss";
    char dataArquivo[11] = "DD/MM/YYYY";
    StaticJsonDocument<128> doc;

    doc["data"] = String(horario.toString(dataArquivo));
    doc["horario"] = String(horario.toString(horarioArquivo));
    doc["sensor1"] = tempExtremidade;
    doc["sensor2"] = tempCentro;
    doc["media"] = (tempExtremidade + tempCentro)/2;
    doc["umidade"] = umidade;

    serializeJsonPretty(doc, Serial);
}
void Dados::work(const char * file){
    atualizar();
    salvarDadosSd(file);
    print();
}

