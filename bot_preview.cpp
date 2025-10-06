#include <iostream>
#include <tgbot/tgbot.h>

using namespace std;

int main() {
    TgBot::Bot bot("7498870186:AAEMPsymxud8rnOJtJYc7QDON2KhLVx1t08");

    bot.getEvents().onCommand("start", [&bot](const TgBot::Message::Ptr& message) {
        bot.getApi().sendMessage(message->chat->id, "Здесь будет реализация меню с кнопками");
    });

    bot.getEvents().onCommand("help", [&bot](const TgBot::Message::Ptr& message) {
        bot.getApi().sendMessage(message->chat->id, "Меню помощи");
    });

    bot.getEvents().onAnyMessage([&bot](const TgBot::Message::Ptr& message) {
        printf("User wrote %s\n", message->text.c_str());
        if (!message->text.empty() && message->text[0] == '/') {
        return;
    }
        bot.getApi().sendMessage(message->chat->id, message->text);
    });
    try {
        cout << "Bot username: " << bot.getApi().getMe()->username.c_str() << endl;
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        cout << "error: " << e.what() << std::endl;
    }
    return 0;
}
