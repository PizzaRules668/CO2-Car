import matplotlib.pyplot as plt
from datetime import datetime
import pandas as pd
import serial
import os

# thrust.py
# 
# Run the thrust.ino file on the ardunio
# Will also record the data from the arduino
# Will comupte Force(Newtons), Acceleration, Velocity(m/s), 
# Will also save to a .csv file, and make a thrust plot
# 

baud = 57600
device = "/dev/ttyACM1"

conn = serial.Serial(port=device, baudrate=baud)

objWeight = 20.2
carMass = 49.5 # Cars Weight in grams
thrusting = False
thrustData = []

# month-day-24hr-min-thrust.csv
fileFormat = "thrustData/%m-%Y-%H-%M-thrust"

def computeData():
    print("Computing Data")

    data = pd.DataFrame(columns=["Time", "Force(grams)", "Force(kilograms)", "Force(newtons)", "Acceleration", "Velocity"])

    lastVelocity = 0
    for tData in thrustData:
        try:
            time, forceG = tData.split(", ")
            time, forceG = int(time), float(forceG)
        
            forceK = forceG/1000
            forceN = forceK*9.81

            acceleration = forceN/(carMass/1000)
            velocity = lastVelocity+acceleration
            lastVelocity = velocity

            data.loc[-1] = [time, forceG, forceK, forceN, acceleration, velocity]
            data.index = data.index + 1
            data = data.sort_index()
        except Exception as e:
            print(tData)
            print(e)

    data = data.sort_index(ascending=False)
    data = data.set_index("Time")

    print(data)
    fileName = datetime.now().strftime(fileFormat)
    data.to_csv(fileName+".csv")

    print(f"Average Thrust {data.loc[:, 'Force(grams)'].mean()} grams")
    print(f"Average Thrust {data.loc[:, 'Force(kilograms)'].mean()} kilograms")
    print(f"Average Thrust {data.loc[:, 'Force(newtons)'].mean()} newtons")
    print()

    print(f"Peak Thrust {data.loc[:, 'Force(grams)'].idxmax()} grams")
    print(f"Peak Thrust {data.loc[:, 'Force(kilograms)'].idxmax()} kilograms")
    print(f"Peak Thrust {data.loc[:, 'Force(newtons)'].idxmax()} newtons")

    plt.plot(data.index, data["Force(grams)"])
    plt.show()
    plt.savefig(fileName+".png")

if __name__ == "__main__":
    if not os.path.exists("thrustData/"):
        os.makedirs("thrustData/")

    print("Serial Connection Open")
    while True:
        try:
            if conn.in_waiting:
                data = conn.readline().decode()
                print(data, end="")

                if "What is the Weight of the object:" in data:
                    print(objWeight)
                    #objWeight = input()
                    conn.write(str(objWeight).encode("utf"))

                elif "IGNITION" in data:
                    thrusting = True
                
                elif "Thrust has stopped" in data:
                    computeData()
                    thrusting = False

                if thrusting:
                    if len(thrustData) == 0:
                        thrustData.append("0, 0")

                    else:
                        thrustData.append(data)

        except KeyboardInterrupt:
            print("Quiting")
            break
