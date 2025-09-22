#!/usr/bin/env python3
"""
fetch_data.py
Download historical stock data for selected S&P500 tickers
and save them into CSV files in the same directory.

Each CSV format:
    First row  -> Symbol (e.g. AAPL)
    Second row -> Column headers (Date,Open,High,Low,Close,Adj Close,Volume)
    Following  -> Historical data rows

Requires:
    pip install yfinance python-dateutil
"""

import os
import yfinance as yf
from datetime import datetime
from dateutil.relativedelta import relativedelta

# List of 10 S&P500 tickers
TICKERS = ["AAPL", "MSFT", "GOOGL", "AMZN", "META",
           "JPM", "JNJ", "NVDA", "PG", "TSLA"]

def get_date_range(years_back=1):
    """Return (start_date, end_date) for historical data."""
    today = datetime.today().date()
    past = today - relativedelta(years=years_back)
    return past, today

def main():
    # Directory where this script lives
    script_dir = os.path.dirname(os.path.abspath(__file__))
    csv_dir = os.path.join(script_dir, "csv")
    os.makedirs(csv_dir, exist_ok=True)

    # Date range
    start_date, end_date = get_date_range(1)
    print(f"Fetching data from {start_date} to {end_date}")

    for ticker in TICKERS:
        print(f"Downloading {ticker} ...")
        try:
            df = yf.download(ticker, start=start_date, end=end_date, auto_adjust=False)
            if df.empty:
                print(f"⚠️ No data retrieved for {ticker}")
                continue

            # Reset index so Date is a normal column
            df.reset_index(inplace=True)

            # Flatten multi-index columns if present
            df.columns = [col if not isinstance(col, tuple) else col[0] for col in df.columns]


            # Save with ticker as first row, headers as second row
            filepath = os.path.join(csv_dir, f"{ticker}_5y.csv")
            with open(filepath, "w", newline="") as f:
                f.write(f"{ticker}\n")  # first row: symbol
                df.to_csv(f, index=False)  # headers + data

            print(f"✅ Saved {ticker} data to {filepath}")

        except Exception as e:
            print(f"❌ Error downloading {ticker}: {e}")

if __name__ == "__main__":
    main()
