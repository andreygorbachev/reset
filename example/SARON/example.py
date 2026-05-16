import csv
import decimal
from decimal import Decimal, ROUND_HALF_UP
from datetime import datetime

decimal.getcontext().prec = 16

date_array = []
saron_array = []
saion_array = []

with open(r"data\hsrron.csv", "r") as f:

    reader = csv.reader(f, delimiter = ';')

    # Safely skip first 4 lines
    for _ in range(4):
        try:
            next(reader)
        except StopIteration:
            raise Exception("File has fewer than 19 lines")

    # Read data rows
    for row in reader:
        date = row[0]
        if date:
            date_array.append(datetime.strptime(date, "%d.%m.%Y"))
        
        saron = row[1]
        if saron:
            saron_array.append(Decimal(saron))
        
        saion = row[5]
        if saion:
            saion_array.append(Decimal(saion))

date_array.reverse()
saron_array.reverse()
saion_array.reverse()

index = Decimal("10000")

prev_date = None
prev_rate = None
for date, rate in zip(date_array, saron_array):
    if prev_date:
        days = (date - prev_date).days
        index = index * (Decimal("1") + Decimal(days) / Decimal("360") * prev_rate / Decimal("100"))
        index = index.quantize(Decimal("0.000000"), ROUND_HALF_UP)

    prev_date = date
    prev_rate = rate

print("For", date_array[-1].strftime("%d.%m.%Y"), "SAION is", saion_array[-1], "and the same computed value is", index)
