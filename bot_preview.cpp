#include <tgbot/tgbot.h>

#include <cstdlib>    // getenv
#include <fstream>    // ofstream
#include <cctype>     // tolower
#include <iostream>

// ------------------ Вспомогательные функции ------------------

// Главное меню
void sendMainMenu(TgBot::Bot& bot, int64_t chatId) {
    std::string messageText =
        "Привет!\n"
        "Этот бот генерирует принципиальную схему функционального блока IEC-61499 "
        "на основе его XML файла";

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

    {
        std::vector<TgBot::InlineKeyboardButton::Ptr> row;
        TgBot::InlineKeyboardButton::Ptr btn(new TgBot::InlineKeyboardButton);
        btn->text = "Загрузить xml";
        btn->callbackData = "load_xml";
        row.push_back(btn);
        keyboard->inlineKeyboard.push_back(row);
    }

    {
        std::vector<TgBot::InlineKeyboardButton::Ptr> row;
        TgBot::InlineKeyboardButton::Ptr btn(new TgBot::InlineKeyboardButton);
        btn->text = "Помощь";
        btn->callbackData = "help";
        row.push_back(btn);
        keyboard->inlineKeyboard.push_back(row);
    }

    {
        std::vector<TgBot::InlineKeyboardButton::Ptr> row;
        TgBot::InlineKeyboardButton::Ptr btn(new TgBot::InlineKeyboardButton);
        btn->text = "О проекте";
        btn->callbackData = "project_about";
        row.push_back(btn);
        keyboard->inlineKeyboard.push_back(row);
    }

    bot.getApi().sendMessage(chatId, messageText, false, 0, keyboard, "Markdown");
}

// Отправка готового результата
void sendXml(TgBot::Bot& bot, const std::string& filePath, int64_t chatId) {
    TgBot::InputFile::Ptr doc = TgBot::InputFile::fromFile(filePath, "application/octet-stream");
    bot.getApi().sendDocument(chatId, doc);

    bot.getApi().sendMessage(
        chatId,
        "Готово, вот финальные файлы принципиальной схемы функционального блока IEC-61499"
    );

    sendMainMenu(bot, chatId);
}

// Заглушка твоей будущей генерации схемы
void generateTrigger(TgBot::Bot& bot, const std::string& xmlPath, int64_t chatId) {
    std::cout << "Генерация по XML: " << xmlPath << std::endl;

    std::string resultPath = "result.png";

    sendXml(bot, resultPath, chatId);
}

// Сохранение XML, полученного от пользователя
void saveXml(TgBot::Bot& bot, TgBot::Message::Ptr message) {
    if (!message->document) {
        return;
    }

    int64_t chatId = message->chat->id;

    TgBot::File::Ptr fileInfo = bot.getApi().getFile(message->document->fileId);

    std::string localPath = "downloaded_xml.fbt";

    std::ofstream ofs(localPath, std::ios::binary);
    if (!ofs.is_open()) {
        bot.getApi().sendMessage(chatId, "Ошибка: не удалось сохранить файл.");
        return;
    }

    bot.getApi().downloadFile(fileInfo->filePath, ofs);
    ofs.close();

    generateTrigger(bot, localPath, chatId);
}

// ------------------ main ------------------

int main() {
    const char* tokenEnv = std::getenv("TOKEN");
    if (!tokenEnv) {
        std::cerr << "Ошибка: переменная окружения TOKEN не установлена.\n";
        return 1;
    }

    TgBot::Bot bot(tokenEnv);

    // /start
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        sendMainMenu(bot, message->chat->id);
    });

    // Кнопки
    bot.getEvents().onCallbackQuery([&bot](TgBot::CallbackQuery::Ptr query) {
        int64_t chatId = query->message->chat->id;
        const std::string& data = query->data;

        if (data == "back_to_main") {
            sendMainMenu(bot, chatId);
        }
        else if (data == "load_xml") {
            TgBot::InlineKeyboardMarkup::Ptr kb(new TgBot::InlineKeyboardMarkup);
            TgBot::InlineKeyboardButton::Ptr btn(new TgBot::InlineKeyboardButton);
            btn->text = "Отмена";
            btn->callbackData = "back_to_main";
            kb->inlineKeyboard.push_back({btn});

            bot.getApi().sendMessage(chatId, "Пришли XML файл функционального блока:", false, 0, kb);
        }
        else if (data == "help") {
            TgBot::InlineKeyboardMarkup::Ptr kb(new TgBot::InlineKeyboardMarkup);
            TgBot::InlineKeyboardButton::Ptr btn(new TgBot::InlineKeyboardButton);
            btn->text = "Назад";
            btn->callbackData = "back_to_main";
            kb->inlineKeyboard.push_back({btn});

            bot.getApi().sendMessage(
                chatId,
                "Инструкция:\n"
                "1. Нажмите \"Загрузить XML\"\n"
                "2. Отправьте XML файл (.fbt)\n"
                "3. Дождитесь генерации PNG и SVG\n\n"
                "По вопросам: @aToTheStars",
                false, 0, kb
            );
        }
        else if (data == "project_about") {
            TgBot::InlineKeyboardMarkup::Ptr kb(new TgBot::InlineKeyboardMarkup);
            TgBot::InlineKeyboardButton::Ptr btn(new TgBot::InlineKeyboardButton);
            btn->text = "Назад";
            btn->callbackData = "back_to_main";
            kb->inlineKeyboard.push_back({btn});

            bot.getApi().sendMessage(
                chatId,
                "Бот создан в рамках курсовой работы.\n"
                "Разработчики:\n"
                "@aToTheStars\n"
                "@MangoWkusnoe",
                false, 0, kb
            );
        }
    });

    // Приём XML файлов
    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        if (!message->document) {
            return;
        }

        int64_t chatId = message->chat->id;
        std::string fileName = message->document->fileName;

        // Перевод в lowercase вручную
        std::string lower = fileName;
        for (char& c : lower) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }

        // Проверка расширения
        bool isFbt = false;
        if (lower.size() >= 4 && lower.substr(lower.size() - 4) == ".fbt") {
            isFbt = true;
        }

        if (isFbt) {
            bot.getApi().sendMessage(chatId, "Обработка файла...");
            saveXml(bot, message);
        } else {
            bot.getApi().sendMessage(chatId, "Неверный формат файла. Пришлите файл .fbt");
        }
    });

    // Запуск
    std::cout << "Бот запущен..." << std::endl;

    try {
        TgBot::TgLongPoll poll(bot);
        while (true) {
            poll.start();
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка long polling: " << e.what() << std::endl;
    }

    return 0;
}