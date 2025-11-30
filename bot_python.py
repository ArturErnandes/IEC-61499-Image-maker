import os
from dotenv import load_dotenv
from telebot import telebot, types

load_dotenv()
token = os.getenv("TOKEN")

bot = telebot.TeleBot(token)


@bot.message_handler(commands=['start'])
def start_proceed(message):
    user_id = message.chat.id
    message_text = '''Привет!
Этот бот генерирует принципиальную схему функционального блока IEC-61499 на основе его XML файла'''

    kb = types.InlineKeyboardMarkup()
    kb.add(types.InlineKeyboardButton('Загрузить xml', callback_data='load_xml'))
    kb.add(types.InlineKeyboardButton('Помощь', callback_data='help'))
    kb.add(types.InlineKeyboardButton('О проекте', callback_data='project_about'))
    bot.send_message(user_id, message_text, reply_markup=kb)


@bot.callback_query_handler(func=lambda call: call.data == "back_to_main")
def start_proceed(call):
    message_text = '''Привет!
Этот бот генерирует принципиальную схему функционального блока IEC-61499 на основе его XML файла'''

    kb = types.InlineKeyboardMarkup()
    kb.add(types.InlineKeyboardButton('Загрузить xml', callback_data='load_xml'))
    kb.add(types.InlineKeyboardButton('Помощь', callback_data='help'))
    kb.add(types.InlineKeyboardButton('О проекте', callback_data='project_about'))
    bot.edit_message_text(chat_id=call.message.chat.id, message_id=call.message.message_id, text=message_text, reply_markup=kb)


@bot.callback_query_handler(func=lambda call: call.data == "load_xml")
def load_xml_proceed(call):
    message_text = "Пришли XML файл с данными функционального блока:"
    kb = types.InlineKeyboardMarkup()
    kb.add(types.InlineKeyboardButton('Отмена', callback_data='back_to_main'))
    bot.edit_message_text(chat_id=call.message.chat.id, message_id=call.message.message_id, text=message_text, reply_markup=kb)


@bot.callback_query_handler(func=lambda call: call.data == "help")
def help_proceed(call):
    message_text = '''Инструкция по использованию бота:
1. В главном меню бота нажать на кнопку "Загрузить XML"
2. После ответа бота загрузить XML файл функционального блока IEC-61499
3. Дождаться генерации файла. Бот представит на выбоор 2 варианта: PNG и SVG

По любым вопросам: @aToTheStars'''
    kb = types.InlineKeyboardMarkup()
    kb.add(types.InlineKeyboardButton('Назад', callback_data='back_to_main'))
    bot.edit_message_text(chat_id=call.message.chat.id, message_id=call.message.message_id, text=message_text, reply_markup=kb)


@bot.callback_query_handler(func=lambda call: call.data == "project_about")
def project_about_proceed(call):
    message_text = '''Бот выполнен в рамках курсовой работы для генерации подробного изображения принципиальной схемы функционального блока IEC-61499 на основе его XML файла.
Разработчики:

@aToTheStars
@MangoWkusnoe'''
    kb = types.InlineKeyboardMarkup()
    kb.add(types.InlineKeyboardButton('Назад', callback_data='back_to_main'))
    bot.edit_message_text(chat_id=call.message.chat.id, message_id=call.message.message_id, text=message_text, reply_markup=kb)


bot.infinity_polling()