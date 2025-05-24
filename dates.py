import pandas as pd
import numpy as np

# Liczba losowych dat
n = 10

# Dzisiejsza data bez czasu
today = pd.Timestamp.now().normalize()

# Przekształcenie na "dni od 1970-01-01" (int32)
today_days = ((today - pd.Timestamp("1970-01-01")) // pd.Timedelta(days=1))
today_int32 = np.int32(today_days)

# Losowe daty ±100 dni względem dziś → jako dni od epoki
random_offsets = np.random.randint(-100, 100, size=n)
dates = today + pd.to_timedelta(random_offsets, unit="D")
dates_days = ((dates - pd.Timestamp("1970-01-01")) // pd.Timedelta(days=1)).astype(np.int32).to_numpy()

# Inne dane
codes = np.random.choice([1, 2, 3, 4, 5], size=n).astype(np.int32)
priority = np.random.choice([0, 1, 2], size=n).astype(np.int32)
retry_count = np.random.randint(0, 5, size=n).astype(np.int32)

# Przekazujesz to do C++
# calculation(dates_days, codes, today_days, priority, retry_count)

print("Daty jako dni od epoki (int32):", dates_days)
print("Dzisiejszy dzień (int32):", today_int32)