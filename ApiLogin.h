//#pragma once
//#ifndef API_LOGIN_H
//#define API_LOGIN_H
//
//#include <string>
//#include <curl/curl.h>  // libcurl�̂��߂̃w�b�_�[
//#include <nlohmann/json.hpp>  // nlohmann::json���g�p����ꍇ
//
//class ApiLogin {
//public:
//    // �R���X�g���N�^�ƃf�X�g���N�^
//    ApiLogin(const std::string& apiUrl);
//    ~ApiLogin();
//
//    // ���O�C���������s�����\�b�h
//    bool login(const std::string& username, const std::string& password);
//
//    // �ی삳�ꂽ�G���h�|�C���g�ɃA�N�Z�X���郁�\�b�h
//    std::string accessProtectedEndpoint(const std::string& endpoint);
//
//private:
//    // �w���p�[�֐�: cURL�̃��X�|���X�f�[�^����������R�[���o�b�N�֐�
//    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* out);
//
//    // API��URL
//    std::string apiUrl;
//
//    // JWT�g�[�N��
//    std::string token;
//
//    // libcurl�p�̏������֐�
//    void initCurl();
//
//    // libcurl�̃N���[���A�b�v�֐�
//    void cleanupCurl();
//
//    // cURL�̃C���X�^���X
//    CURL* curl;
//};
//
//#endif // API_LOGIN_H
//