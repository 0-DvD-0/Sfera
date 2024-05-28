import telegram 
import sys
import asyncio

notification = sys.argv[1]
sferaid = -4288492585
api = '7080062053:AAFMxsmew0aqOgN_J6YOzHfU1mikgKJ7GMg'


async def main():
    bot = telegram.Bot(api)
    async with bot:
        await bot.send_message(text=notification, chat_id=sferaid)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print ('\n Usage: python3 Notifications.py "Message"')
        sys.exit()
    asyncio.run(main())
