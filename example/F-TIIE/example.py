import csv
import decimal
from decimal import Decimal, ROUND_HALF_UP

decimal.getcontext().prec = 34

ftiie_array = []
tiie_array = []

with open(r"data\CF101.csv", "r") as f:

    reader = csv.reader(f)

    # Safely skip first 19 lines
    for _ in range(19):
        try:
            next(reader)
        except StopIteration:
            raise Exception("File has fewer than 19 lines")

    # Read data rows
    prev_ftiie = None
    for row in reader:
        ftiie = row[2]

        if ftiie:
            if ftiie in ("N/E"):
                ftiie = prev_ftiie

            ftiie_array.append(Decimal(ftiie))

            prev_ftiie = ftiie

        tiie = row[6]

        if tiie:
            tiie_array.append(Decimal(tiie))

ftiie_array.pop()

index = Decimal("100000")

for rate in ftiie_array:
    index = index * (Decimal("1") + Decimal("1") / Decimal("360") * rate / Decimal("100"))
    index = index.quantize(Decimal("0.0000000000000000"), ROUND_HALF_UP)

index = index.quantize(Decimal("0.0000"), ROUND_HALF_UP)

print("Overnight Funding TIIE Index, Compounded on calendar days is", tiie_array[-1], "and the same computed value is", index)
