import os
import telebot
from dotenv import load_dotenv
from telebot import TeleBot








def main():
    load_dotenv()
    token = os.getenv("TOKEN")

    bot = telebot.TeleBot(token)
    bot.infinity_polling()