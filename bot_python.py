import os
from dotenv import load_dotenv
from telebot import telebot, types


load_dotenv()
token = os.getenv("TOKEN")

bot = telebot.TeleBot(token)


def send_xml(file, user_id):
    print("загрузка")

    with open(file, "rb") as result:
        bot.send_document(user_id, result)
    message_text = "Готово, вот финальные файлы принципиальной схемы функционального блока IEC-61499"
    bot.send_message(user_id, message_text)

    message_text = "Привет!\nЭтот бот генерирует принципиальную схему функционального блока IEC-61499 на основе его XML файла"
    kb = types.InlineKeyboardMarkup()
    kb.add(types.InlineKeyboardButton('Загрузить xml', callback_data='load_xml'))
    kb.add(types.InlineKeyboardButton('Помощь', callback_data='help'))
    kb.add(types.InlineKeyboardButton('О проекте', callback_data='project_about'))
    bot.send_message(user_id, message_text, reply_markup=kb)


def generate_trigger(file, user_id):
    #result = generate_photo(file) Вызов части с обработкой xml
    result = "result.png" #Пока так, потом изменить
    send_xml(result, user_id)


def save_xml(file, user_id):
    file_info = bot.get_file(file.file_id)
    download = bot.download_file(file_info.file_path)
    path = "downloaded_xml"

    with open(path, 'wb') as f:
        f.write(download)

    generate_trigger(path, user_id)


@bot.message_handler(commands=['start'])
def start_proceed(message):
    user_id = message.chat.id
    message_text = "Привет!\nЭтот бот генерирует принципиальную схему функционального блока IEC-61499 на основе его XML файла"

    kb = types.InlineKeyboardMarkup()
    kb.add(types.InlineKeyboardButton('Загрузить xml', callback_data='load_xml'))
    kb.add(types.InlineKeyboardButton('Помощь', callback_data='help'))
    kb.add(types.InlineKeyboardButton('О проекте', callback_data='project_about'))
    bot.send_message(user_id, message_text, reply_markup=kb)


@bot.callback_query_handler(func=lambda call: call.data == "back_to_main")
def start_proceed(call):
    message_text = "Привет!\nЭтот бот генерирует принципиальную схему функционального блока IEC-61499 на основе его XML файла"

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
    message_text = 'Инструкция по использованию бота:\n1. В главном меню бота нажать на кнопку "Загрузить XML"\n2. После ответа бота загрузить XML файл функционального блока IEC-61499\n3. Дождаться генерации файла. Бот представит 2 варианта: PNG и SVG\n\nПо любым вопросам: @aToTheStars'
    kb = types.InlineKeyboardMarkup()
    kb.add(types.InlineKeyboardButton('Назад', callback_data='back_to_main'))
    bot.edit_message_text(chat_id=call.message.chat.id, message_id=call.message.message_id, text=message_text, reply_markup=kb)


@bot.callback_query_handler(func=lambda call: call.data == "project_about")
def project_about_proceed(call):
    message_text = "Бот выполнен в рамках курсовой работы для генерации подробного изображения принципиальной схемы функционального блока IEC-61499 на основе его XML файла.\nРазработчики:\n@aToTheStars\n@MangoWkusnoe"
    kb = types.InlineKeyboardMarkup()
    kb.add(types.InlineKeyboardButton('Назад', callback_data='back_to_main'))
    bot.edit_message_text(chat_id=call.message.chat.id, message_id=call.message.message_id, text=message_text, reply_markup=kb)


@bot.message_handler(content_types=['document'])
def handle_document(message):
    file_name = message.document.file_name.lower()

    if file_name.endswith('.fbt'):
        bot.send_message(message.chat.id, "Обработка файла...")
        save_xml(message.document, message.chat.id)
    else:
        bot.send_message(message.chat.id, "Файл неверного формата\nПришлите файл с расширением .fbt")


print("Бот запущен...")
bot.infinity_polling()