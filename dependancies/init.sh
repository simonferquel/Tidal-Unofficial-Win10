if [ ! -d "cpprestsdk" ]; then
  git clone https://github.com/simonferquel/cpprestsdk.git
  cd cpprestsdk
  git pull origin static_lib_uwp
  cd ..
fi
if [ ! -d "UWP-LocalDB-CPP" ]; then
  git clone https://github.com/simonferquel/UWP-LocalDB-CPP.git
fi

if [ ! -d "date" ]; then
  git clone https://github.com/HowardHinnant/date.git
fi
