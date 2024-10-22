//#pragma once
//#ifndef API_LOGIN_H
//#define API_LOGIN_H
//
//#include <string>
//#include <curl/curl.h>  // libcurlのためのヘッダー
//#include <nlohmann/json.hpp>  // nlohmann::jsonを使用する場合
//
//class ApiLogin {
//public:
//    // コンストラクタとデストラクタ
//    ApiLogin(const std::string& apiUrl);
//    ~ApiLogin();
//
//    // ログイン処理を行うメソッド
//    bool login(const std::string& username, const std::string& password);
//
//    // 保護されたエンドポイントにアクセスするメソッド
//    std::string accessProtectedEndpoint(const std::string& endpoint);
//
//private:
//    // ヘルパー関数: cURLのレスポンスデータを処理するコールバック関数
//    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* out);
//
//    // APIのURL
//    std::string apiUrl;
//
//    // JWTトークン
//    std::string token;
//
//    // libcurl用の初期化関数
//    void initCurl();
//
//    // libcurlのクリーンアップ関数
//    void cleanupCurl();
//
//    // cURLのインスタンス
//    CURL* curl;
//};
//
//#endif // API_LOGIN_H
//