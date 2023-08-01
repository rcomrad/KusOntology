// #ifndef ERROR_MESSAGE_HPP
// #define ERROR_MESSAGE_HPP

// #include <iostream>
// #include <memory>
// #include <string>
// #include <unordered_map>

// namespace dom
// {
// class Log
// {
// public:
//     static Log& getInstance() noexcept;

//     template <typename... Args>
//     void writeInfo(Args&&... args) noexcept
//     {
//     }

//     template <typename... Args>
//     void writeWarning(Args&&... args) noexcept
//     {
//     }

//     template <typename... Args>
//     void writeError(Args&&... args) noexcept;

//     // template <typename... Args>
//     // void writeMessage(std::unordered_map<std::string, std::string> aTags,
//     //                   Args&&... args) noexcept
//     // {

//     // }

// private:
//     enum class Type
//     {
//         Nun,
//         Info,
//         Warning,
//         Error
//     };

//     std::unordered_map<std::string, Type> mStrTotype;
//     std::unordered_map<Type, std::unique_ptr<std::ostream>> mStreams;

//     Log() noexcept;
// };

// // template <typename... Args>
// // void
// // endLogBlock(Args&&... args) noexcept
// // {
// //     write(mLogStream, std::forward<Args>(args)...);
// //     --mLogBlockCount;
// // }

// // template <typename... Args>
// // void
// // writeLog(Args&&... args) noexcept
// // {
// //     write(mLogStream, std::forward<Args>(args)...);
// // }

// // template <typename... Args>
// // void
// // writeError(Args&&... args) noexcept
// // {
// // #ifdef ERRORS_TO_LOG_OUTPUT
// //     write(mErrorStream, "ERROR", std::forward<Args>(args)...);
// // #else
// //     basicWrite(mErrorStream, "ERROR", std::forward<Args>(args)...);
// // #endif
// // }

// // void
// // writeLogEndl() noexcept;

// // //--------------------------------------------------------------------------------

// // #ifdef BILL_WINDOWS
// // /*
// // \brief Gets error of WinAPI.
// // \return String with error messege.
// // */
// // std::string
// // GetLastWinAPIError() noexcept;
// // #endif

// // private:
// // int8_t mLogBlockCount;

// // //--------------------------------------------------------------------------------

// // Message() noexcept;

// // template <typename... Args>
// // void
// // write(std::ostream* aStream, Args&&... args) noexcept
// // {
// //     basicWrite(aStream, std::string(mLogBlockCount, '\t'),
// //                std::forward<Args>(args)...);
// // }

// // template <typename... Args>
// // void
// // basicWrite(std::ostream* aStream, Args&&... args) noexcept
// // {
// //     std::string str;
// //     ((void)(str += toString(std::forward<Args>(args)) + " ", 0), ...);
// //     str += '\n';

// //     (*aStream) << str;
// //     (*aStream).flush();
// // }

// // template <typename S, typename = enableIfSame<S, std::string>>
// // std::string
// // toString(S&& str) noexcept
// // {
// //     return std::forward<S>(str);
// // }

// // std::string
// // toString(const char* str) noexcept
// // {
// //     return std::string(str);
// // }

// // std::string
// // toString(int64_t num) noexcept
// // {
// //     return std::to_string(num);
// // }
// // };
// // } // namespace dom

// // //--------------------------------------------------------------------------------

// // /*
// // ERROR FORMAT: <file or class name>, <function>,
// //                 <approximate error number>, <error message>
// // */

// // #ifdef _DBG_
// // #    define START_LOG_BLOCK(...) \
// //         dom::Message::globalMessages.startLogBlock(__VA_ARGS__)
// // #    define END_LOG_BLOCK(...) \
// //         dom::Message::globalMessages.endLogBlock(__VA_ARGS__)
// // #    define WRITE_LOG(...) dom::Message::globalMessages.writeLog(__VA_ARGS__)
// // #    define WRITE_LOG_ENDL(...) \
// //         dom::Message::globalMessages.writeLogEndl(__VA_ARGS__)
// // #    define WRITE_ERROR(...) \
// //         dom::Message::globalMessages.writeError(__VA_ARGS__)
// // #else
// // #    define START_LOG_BLOCK(...) void(0)
// // #    define END_LOG_BLOCK(...)   void(0)
// // #    define WRITE_LOG(...)       void(0)
// // #    define WRITE_LOG_ENDL(...)  void(0)
// // #    define WRITE_ERROR(...)     void(0)
// // #endif

// // //--------------------------------------------------------------------------------

// // #undef LOGS_OUTPUT_TYPE
// // #undef ERRORS_OUTPUT_TYPE

// // //--------------------------------------------------------------------------------
// } // namespace dom
// #endif // !ERROR_MESSAGE_HPP
