#pragma once

#include <cstdint>
#include <string>
#include <tgbot/tgbot.h>

void sendMainMenu(TgBot::Bot& bot, int64_t chatId);
void ensureImagesDir();
void sendResults(TgBot::Bot& bot, const std::string& pngPath, const std::string& svgPath, int64_t chatId);
void generateTrigger(TgBot::Bot& bot, const std::string& xmlPath, const std::string& baseName, int64_t chatId);
void saveXml(TgBot::Bot& bot, TgBot::Message::Ptr message);