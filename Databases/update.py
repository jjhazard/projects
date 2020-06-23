from darksky import forecast
from datetime import datetime, date, timedelta
import pandas as pd
import MySQLdb
import pyodbc
import re

#API Key
DARK_SKY_KEY = 'ce4149755b8988664a910e0ba7f9e5d1'
#DARK_SKY_KEY = 'cd76150a325d13271dab874702496514'

#pull the location data from the mountains file
def getCoordinates():
    return [['Baker Mountain', 45.0847, -69.9056],
['Big Rock', 46.5154, -67.8661],
['Big Squaw', 45.5067, -69.702],
['Black Mountain of Maine', 44.577, -70.6133],
['Camden Snow Bowl', 44.2173, -69.1347],
['Eaton Mountain', 44.7681, -69.6194],
['Hermon Mountain', 44.804, -68.9197],
['Lonesome Pine Trails', 47.2587, -68.5897],
['Mount Abram', 44.3798, -70.7069],
['Mount Jefferson Ski Area', 45.3523, -68.2822],
['Powderhouse Hill', 43.2369, -70.8022],
['Quoggy Jo', 46.721, -67.9323],
['Saddleback Maine', 44.9367, -70.5031],
['Shawnee Peak', 44.059, -70.8155],
['Sugarloaf', 45.0314, -70.3131],
['Sunday River', 44.4734, -70.8569],
['Titcomb Mountain', 44.65, -70.1708],
['Abenaki Ski Area', 43.6132, -71.2319],
['Arrowhead', 43.3637, -72.3338],
['Attitash', 44.0828, -71.2294],
['Black Mountain', 44.2253, 71.1556],
['Bretton Woods', 44.2597, -71.4622],
['Cannon Mountain', 44.1565, -71.6984],
['Campton Mountain', 43.8473, -71.5806],
['Cranmore Mountain Resort', 44.0565, -71.1102],
['Crotched Mountain', 42.9984, -71.8737],
['Dartmouth Skiway', 43.7877, -72.0995],
['Franklin Veterans Memorial Recreation Area', 43.4324, -71.7327],
['Granite Gorge', 42.971, -72.212],
['Gunstock Mountain Resort', 43.5256, -71.3784],
['Kanc Rec Area', 44.0513, -71.6602],
['King Pine', 43.8714, -71.0894],
['Loon Mountain', 44.036, -71.6214],
['McIntyre Ski Area', 43.005, -71.44],
['Mount Eustis', 45.1808, -70.4578],
['Mount Prospect', 44.4411, -71.5804],
['Mount Sunapee Resort', 43.3876, -72.0879],
['Pats Peak', 43.1623, -71.7962],
['Ragged Mountain', 43.4701, -71.8375],
['Red Hill Ski Club', 43.7484, -71.41501],
['Storrs Hill', 43.6352, -72.2521],
['Tenney Mountain Ski Resort', 43.7378, -71.7836],
['Waterville Valley Resort', 43.9591, -71.5233],
['Whaleback', 44.0872, -71.6194],
['Wildcat Mountain', 44.2641, 71.2394],
['Bolton Valley Resort', 44.42107, -72.84948],
['Bromley Mountain', 43.2279, -72.9387],
['Burke Mountain', 44.5878, -71.9164],
['Cochrans Ski Area', 44.396568, -72.982147],
['Harrington Hill', 41.9301, -72.0551],
['Hard Ack', 44.8153, -73.0667],
['Haystack', 42.914, -72.9195],
['Jay Peak Resort', 44.9649, -72.4602],
['Killington Ski Resort', 43.6776, -72.7798],
['Living Memorial Park', 42.848, -72.5865],
['Lyndon Outing Club', 44.5331, -71.987297],
['Mad River Glen', 44.2025, -72.9175],
['Magic Mountain', 43.20179, -72.772697],
['Middlebury College Snow Bowl', 43.9392, -72.9576],
['Mount Snow', 42.9602, -72.9204],
['Okemo Mountain', 43.4018, -72.717],
['Pico Mountain', 43.6621, -72.8425],
['Plymouth Notch', 43.5303, -72.7386],
['Quechee Lakes Ski Area', 43.6551767, -72.4422527],
['Smugglers Notch', 44.5885, -72.79],
['Stowe Mountain Resort', 44.5303, -72.7814],
['Stratton Mountain Resort', 43.1134, -72.9081],
['Sugarbush Resort', 44.1359, -72.8944],
['Suicide Six', 43.6651, -72.5433]]

#get the snowfall for both lists
def averageSnow(summary):
    if "snow" in summary:
        temp = re.findall(r'\d+', summary)
        if(len(temp) == 2):
            return ((int(temp[0])+int(temp[1]))/2)
        elif(len(temp) == 1):
            return int(temp[0])
    #no snow, return 0 inches
    return 0

def updateTable(db, cursor, MID, weather, attributes, table):
    td = timedelta(hours=4)
    index = 0
    for entry in weather.data:
        Query = "REPLACE INTO " + table + " VALUES (" + str(MID) + "," + str(index) + ","       
        for attribute in attributes:
            if attribute is 'precipType':
                try:
                    Query = Query + "'" + entry[attribute] + "',"
                except:
                    Query = Query + "'none',"
            elif 'Time' in attribute:
                Query = Query + "'" + (datetime.utcfromtimestamp(entry[attribute])-td).strftime("%H:%M") + "',"
            else:
                Query = Query + str(entry[attribute]) + ","
        cursor.execute(Query + str(averageSnow(entry['summary'])) + ");")
        db.commit()
        index += 1

def main():
    #log into database
    db = MySQLdb.connect('localhost', # The Host
                         'BaseUser', # username
                         'password', # password
                         'mydb') # name of the data base
    cursor = db.cursor()
    
    #get mountain info
    coordinates = getCoordinates()
    MID = 1
    hourlyAttributes = ['precipIntensity','precipProbability','precipType','temperature','apparentTemperature','humidity','windSpeed','windGust','windBearing','cloudCover','visibility']
    dailyAttributes = ['sunriseTime','sunsetTime','precipIntensity','precipIntensityMax','precipProbability','precipType','temperatureHigh','temperatureLow','humidity','windSpeed','windGust','windGustTime','windBearing','visibility']
    
    #update each mountain's hourly and daily table entries
    for mountain in coordinates:
        weather = forecast(DARK_SKY_KEY, mountain[1], mountain[2])
        updateTable(db, cursor, MID, weather.hourly, hourlyAttributes, 'Hourly')
        updateTable(db, cursor, MID, weather.daily, dailyAttributes, 'Daily')
        MID += 1
        print("Updated " + mountain[0])

    print("Database successfully updated!")
