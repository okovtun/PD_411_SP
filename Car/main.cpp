#include<Windows.h>
#include<iostream>
#include<conio.h>
#include<thread>
using namespace std::chrono_literals;
using std::cin;
using std::cout;
using std::endl;

#define Enter	13
#define Escape	27

#define MIN_TANK_VOLUME	25
#define MAX_TANK_VOLUME	125
class Tank
{
	const int VOLUME;
	double fuel_level;
public:
	int get_VOLUME()const
	{
		return VOLUME;
	}
	double get_fuel_level()const
	{
		return fuel_level;
	}
	void fill(double fuel)
	{
		if (fuel < 0)return;
		fuel_level += fuel;
		if (fuel_level > VOLUME)fuel_level = VOLUME;
	}
	double give_fuel(double amount)
	{
		fuel_level -= amount;
		if (fuel_level < 0)fuel_level = 0;
		return fuel_level;
	}
	Tank(int volume):
		VOLUME
		(
			volume < MIN_TANK_VOLUME ? MIN_TANK_VOLUME :
			volume > MAX_TANK_VOLUME ? MAX_TANK_VOLUME :
			volume
		)
	{
		//this->VOLUME = volume;

		//l-value = r-value;
		
		this->fuel_level = 0;
		cout << "Tank is ready:\t" << this << endl;
	}
	~Tank()
	{
		cout << "Tank is over:\t" << this << endl;
	}
	void info()const
	{
		cout << "Volume:\t" << VOLUME << " liters\n";
		cout << "Fuel:\t" << get_fuel_level() << " liters\n";
	}
};

#define MIN_ENGINE_CONSUMPTION	 5
#define MAX_ENGINE_CONSUMPTION	25
class Engine
{
	double consumption_per_second;
	bool is_started;
public:
	const double CONSUMPTION;
	double get_consumption_per_second()const
	{
		return consumption_per_second;
	}
	Engine(double consumption) :
		CONSUMPTION
		(
			consumption < MIN_ENGINE_CONSUMPTION ? MIN_ENGINE_CONSUMPTION :
			consumption > MAX_ENGINE_CONSUMPTION ? MAX_ENGINE_CONSUMPTION :
			consumption
		)
	{
		consumption_per_second = CONSUMPTION * 3e-5;
		is_started = false;
		cout << "Engine is ready:\t" << this << endl;
	}
	~Engine()
	{
		cout << "Engine is over:\t" << this << endl;
	}
	void start()
	{
		is_started = true;
	}
	void stop()
	{
		is_started = false;
	}
	bool started()const
	{
		return is_started;
	}
	void info()const
	{
		cout << "Consumption per 100 km:\t" << CONSUMPTION << " liters.\n";
		cout << "Consumption per 1 sec:\t" << get_consumption_per_second() << " liters.\n";
	}
};

#define MAX_SPEED_LOWER_LIMIT	 40
#define MAX_SPEED_UPPER_LIMIT	400
class Car
{
	Engine engine;
	Tank tank;
	const int MAX_SPEED;
	int speed;
	bool driver_inside;
	struct //Threads
	{
		std::thread panel_thread;
		std::thread engine_idle_thread;
	}threads;
public:
	Car(double consumption, int volume, int max_speed) :
		engine(consumption),
		tank(volume),
		speed(0),
		MAX_SPEED
		(
			max_speed < MAX_SPEED_LOWER_LIMIT ? MAX_SPEED_LOWER_LIMIT :
			max_speed > MAX_SPEED_UPPER_LIMIT ? MAX_SPEED_UPPER_LIMIT :
			max_speed
		)
	{
		driver_inside = false;
		cout << "Your car is redy:-)\t" << this << endl;
	}
	~Car()
	{
		cout << "Your car is over:-(\t" << this << endl;
	}
	void get_in()
	{
		driver_inside = true;
		//panel();
		threads.panel_thread = std::thread(&Car::panel, this);
	}
	void get_out()
	{
		driver_inside = false;
		if (threads.panel_thread.joinable())threads.panel_thread.join();
		system("CLS");
		cout << "Wall Street" << endl;
	}
	void start()
	{
		if (driver_inside && tank.get_fuel_level())
		{
			engine.start();
			threads.engine_idle_thread = std::thread(&Car::engine_idle, this);
		}
	}
	void stop()
	{
		engine.stop();
		if (threads.engine_idle_thread.joinable())threads.engine_idle_thread.join();
	}
	void control()
	{
		cout << "Press 'Enter' to get in" << endl;
		char key;
		do
		{
			key = 0;
			if(_kbhit())key = _getch();	//������� ������� ������� � ���������� ASCII-��� ������� �������.
			switch (key)
			{
			case Enter:
				if (driver_inside)get_out();
				else get_in();
				break;
			case 'F':
			case 'f':
				double amount;
				cout << "How much do you want? "; cin >> amount;
				tank.fill(amount);
				break;
			case 'I':	//Ignition - ���������
			case 'i':
				if (engine.started())stop();
				else start();
				break;
			case Escape:
				stop();
				get_out();
			}
			if (tank.get_fuel_level() == 0 && threads.engine_idle_thread.joinable())stop();
		} while (key != Escape);
	}
	void engine_idle()
	{
		while (engine.started() && tank.give_fuel(engine.get_consumption_per_second()))
			std::this_thread::sleep_for(1s);
	}
	void panel()const
	{
		while (driver_inside)
		{
			system("CLS");
			cout << "Fuel level:\t" << tank.get_fuel_level() << " liters.";
			if (tank.get_fuel_level() < 5)
			{
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, 0x4F);
				cout << " LOW FUEL ";
				SetConsoleTextAttribute(hConsole, 0x07);
			}
			cout << endl;
			cout << "Engine is " << (engine.started() ? "started" : "stopped") << endl;
			std::this_thread::sleep_for(100ms);
		}
	}
	void info()const
	{
		engine.info();
		tank.info();
		cout << "Max speed:\t" << MAX_SPEED << " km/h\n";
	}
};

//#define TANK_CHECK
//#define ENGINE_CHECK

void main()
{
	setlocale(LC_ALL, "");
	/*const int a=0;
	a = 2;
	cout << a << endl;*/

#ifdef TANK_CHECK
	Tank tank(-30);
	tank.info();
	double fuel;
	do
	{
		cout << "������� ����� �������: "; cin >> fuel;
		tank.fill(fuel);
		tank.info();
	} while (fuel > 0);
#endif // TANK_CHECK

#ifdef ENGINE_CHECK
	Engine engine(10);
	engine.info();
#endif // ENGINE_CHECK

	Car bmw(10, 80, 270);
	bmw.info();
	bmw.control();

}