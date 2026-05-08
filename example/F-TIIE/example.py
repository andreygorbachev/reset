import csv
from decimal import Decimal
from decimal import ROUND_HALF_UP

ftiie_array = []

with open(r"data\CF101.csv", "r") as f:

    reader = csv.reader(f)

    # Safely skip first 19 lines
    for _ in range(19):
        try:
            next(reader)
        except StopIteration:
            raise Exception("File has fewer than 19 lines")

    # Read data rows
    prev_ftiie = "N/E"
    for row in reader:
        if len(row) >= 3:
            ftiie = row[2]

            if ftiie in ("N/E"):
                ftiie = prev_ftiie

            if ftiie:
                ftiie_array.append(Decimal(ftiie))

            prev_ftiie = ftiie

ftiie_array.pop()
ftiie_array.pop() # to match what we currently calculate in the cpp example

index = Decimal("100000")

for rate in ftiie_array:
    index = index * (Decimal("1") + Decimal("1") / Decimal("360") * rate / Decimal("100"))
    index = index.quantize(Decimal("0.000000000000"))

index = index.quantize(Decimal("0.0000"))

print(index)
