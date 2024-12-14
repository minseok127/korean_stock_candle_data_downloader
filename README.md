**This repository was created during my undergraduate years to download and continously update data from the korean stock market. It is based on the Dasihin Securities API and the Qt framework. It uses SQLite3 as the database.**

**You can retrieve data for all stocks in the KOSPI and KOSDAQ markets for the maximum period provided by Daishin Securities. Available timeframes include monthly, weekly, daily, and intraday candles such as 30, 15, 5, 3, and 1-minute intervals.**

**Each data file is named according to the market and candle type. For example, KOSDAQ_30MIN.db represents 30-minute candle data for all stocks in the KOSDAQ market. These files can be accessed using SQLite3. Each file contains tables where the table names corresponds to stock codes, with each table holding candlestick data by date and time. Note that files with INDEX in their name represent market index data.**
