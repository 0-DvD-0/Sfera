import requests
import json
import sys

if len(sys.argv) < 3:
    print ('\n Usage: python3 Notifications.py "Title" "Message"')
    sys.exit()


api_key = 'Q1G3SAW2I5HRMWNCD5OU5M943'
notification_title = sys.argv[1]
notification_message = sys.argv[2]
url = "https://www.notifymydevice.com/push"
data = {"ApiKey": api_key, "PushTitle": notification_title,"PushText": notification_message}
headers = {'Content-Type': 'application/json'}
r = requests.post(url, data=json.dumps(data), headers=headers)

if r.status_code == 200:
    print ('Notification sent!')
else:
    print ('Error while sending notificaton!')
