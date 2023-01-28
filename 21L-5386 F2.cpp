#include <iostream>
using namespace std;

template <class type>
class list
{

private:

	class node
	{

	public:

		type data;
		node* next;
		node()
		{
			next = NULL;
		}
		node(const type v, node* ptr)
		{
			data = v;
			next = ptr;
		}
		node(const type* v, node* ptr)
		{
			data = *v;
			next = ptr;
		}
		~node()
		{

		}


	};
	node* head;
	node* tail;
	int size;

public:

	list()
	{
		tail = new node(type(), NULL);//dummies
		head = new node(type(), tail);
		size = 0;
	}

	void insertAtStart(type const element)
	{
		head->next = new node(element, head->next);
		size++;
	}

	void insertAtEnd(int s, int ind)
	{
		node* temp = head;
		for (; temp->next != tail; temp = temp->next);//inserts at node before dummy tail
		temp->next = new node(type(ind, s), temp->next);
		size++;
	}


	bool checkPool(int s)//used to check if memory is available in pool
	{
		node* temp = head->next;
		for (; temp != tail; temp = temp->next)
		{
			if (temp->data.total_bytes >= s)
			{
				return true;
			}
		}
		return false;
	}

	int getPoolMem(int s, bool strat)//used to return starting byte of available mmeory block in pool  
	{
		node* temp = head->next;
		node* del = head;
		int firstind, bestind = 0, counter = 0;
		for (; temp != tail; temp = temp->next)
		{
			if (strat == true)//first fit
			{
				if (temp->data.total_bytes >= s)
				{
					firstind = temp->data.start_byte_ID;
					temp->data.start_byte_ID = (temp->data.start_byte_ID + s);
					temp->data.total_bytes = (temp->data.total_bytes - s);
					if (temp->data.total_bytes == 0)
					{
						del->next = del->next->next;
						delete temp;
					}
					return firstind;
				}
			}
			else
			{
				if (temp->data.total_bytes >= s)//loop for finding best fit
				{
					counter++;
					if (counter == 1)
					{
						bestind = temp->data.total_bytes;
					}
					else if (bestind > temp->data.total_bytes)
					{
						bestind = temp->data.total_bytes;
					}
				}
			}
			del = del->next;
		}
		del = head;
		for (temp = head->next; temp != tail; temp = temp->next)//loop to implement best fit
		{
			if (temp->data.total_bytes == bestind)
			{
				bestind = temp->data.start_byte_ID;
				temp->data.start_byte_ID = (temp->data.start_byte_ID + s);
				temp->data.total_bytes = (temp->data.total_bytes - s);
				if (temp->data.total_bytes == 0)
				{
					del->next = del->next->next;
					delete temp;
				}
				return bestind;
			}
			del = del->next;
		}
	}

	template <class type>
	void givePoolMem(list<type>& p)//deallocation of programs current memory blocks back into pool
	{
		node* pooltemp = p.head->next;
		node* progtemp = head->next;
		node* swap = p.head;
		int i;
		for (; progtemp != tail; )//inserts sorted program blocks back into pool
		{
			if (p.head->next == p.tail)
			{
				swap->next = new node(type(progtemp->data.start_byte_ID, progtemp->data.total_bytes), swap->next);
				pooltemp = swap->next;
				progtemp = progtemp->next;
				p.size++;
			}
			else if (progtemp->data.start_byte_ID < pooltemp->data.start_byte_ID || pooltemp == p.tail)
			{
				swap->next = new node(type(progtemp->data.start_byte_ID, progtemp->data.total_bytes), swap->next);
				pooltemp = swap->next;
				p.size++;
				progtemp = progtemp->next;
			}
			else if (progtemp->data.start_byte_ID > pooltemp->data.start_byte_ID)
			{
				pooltemp = pooltemp->next;
				swap = swap->next;
			}
		}
		swap = p.head->next;
		for (pooltemp = p.head->next->next; pooltemp != p.tail; pooltemp = pooltemp->next)//loop for merging pool blocks if they are consecutive
		{
			if (pooltemp == NULL)
			{
				break;
			}
			if ((swap->data.start_byte_ID + swap->data.total_bytes) == pooltemp->data.start_byte_ID)
			{
				swap->data.total_bytes = (swap->data.total_bytes + pooltemp->data.total_bytes);
				swap->next = swap->next->next;
				delete pooltemp;
				pooltemp = swap;
				p.size--;
			}
			else
			{
				swap = swap->next;
			}

		}
	}


	void addProgMemory(string id, int s, int ind)
	{
		node* temp = head;
		for (; temp != tail; temp = temp->next)//appends or creates program and then appends its memory
		{
			if (temp->data.getid() == id)//if exists then this run
			{
				temp->data.addMemory(s, ind);
				return;
			}
			if (temp->next == tail)//else this runs
			{
				temp->next = new node(new type(id, s, ind), temp->next);
				return;
			}

		}
	}

	template <class type>
	bool deleteProg(string id, list<type>& p)//deletes prgram from progs list after deallocating its memory to pool
	{
		node* temp = head->next;
		node* del = head;
		for (; temp != tail; temp = temp->next)
		{
			if (temp->data.getid() == id)//searches by id and deletes program
			{
				del->next = del->next->next;
				temp->data.deallocateMem(p);//pool passed as to be available in same function with program block list
				delete temp;
				return true;
			}
			del = del->next;
		}
		return false;
	}

	void printprogs()//prings entire progs list
	{
		node* temp = head->next;
		cout << endl;
		if (temp == tail)
		{
			cout << "\nNo programs currently running. \n\n";
		}
		for (; temp != tail; temp = temp->next)
		{
			temp->data.showProg();
		}
		cout << endl;
	}


	void printblocks()//generic block print function called upon by pool and progs
	{
		node* temp = head->next;
		for (; temp != tail; temp = temp->next)
		{
			cout << temp->data.start_byte_ID << " - " << temp->data.total_bytes << " \n\n";
		}
		cout << endl;
	}

	~list()
	{
		if (size != 0)
		{
			node* temp = head->next;
			node* del = head;
			for (; temp != NULL; temp = temp->next)
			{
				delete del;
				del = temp;
			}
			delete del;
			head = tail = NULL;
			size = 0;
		}
	}

};

class block
{
private:

	template <class type>//friend class list so that list can freely access block data members
	friend class list;
	int start_byte_ID;
	int total_bytes;

public:

	block()
	{
		start_byte_ID = total_bytes = 0;
	}

	block(int s, int t)
	{
		start_byte_ID = s;
		total_bytes = t;
	}

	~block()
	{

	}

};

class program
{
private:

	string id;
	int size;
	list <block>blocklist;

public:

	program()
	{
		id = "\0";
		size = 0;
	}

	program(string i, int s, int ind)
	{
		id = i;
		size = 0;
		this->addMemory(s, ind);//adds memory block upon creation
	}

	string getid()
	{
		return id;
	}

	void addMemory(int s, int ind)
	{
		blocklist.insertAtEnd(s, ind);//appends memory to program from pool
		size += s;
	}

	template <class type>
	void deallocateMem(list<type>& p)
	{
		blocklist.givePoolMem(p);//deallocates its memory bcak to pool before deletion
	}

	void showProg()
	{
		cout << "Program id: " << id << " \t\t Total memory occupied: " << size << "\n\n";
		cout << "Blocks in use: \n\n";
		blocklist.printblocks();//shows current memory blocks in use by program
	}

	~program()
	{

	}
};

class memorymanagementsystem
{
private:

	list <block>pool;
	list <program>progs;
	int sizeOfmemory;  //1kb = 1000 bytes
	bool strategy;  //true = first fit, false = best fit

public:

	memorymanagementsystem()
	{
		sizeOfmemory = 0;
		strategy = false;
	}

	bool GetMem(string id, int size)
	{
		if (pool.checkPool(size))//checks if memory is available for allocation
		{
			int ind;
			ind = pool.getPoolMem(size, strategy);//returns index of memory allocation and correctly adjusts pool
			progs.addProgMemory(id, size, ind);//checks if program already exists or needs to be created and appends memory from pool
			return true;
		}
		else
		{
			return false;
		}
	}

	bool removeMem(string id)
	{
		return progs.deleteProg(id, pool);//deletes a program and deallocates its memeory to pool
	}

	void run()
	{
		bool run = true;
		int dec = 0;
		cout << "Welcome to memory management system : RAM\n\n";
		cout << "Set strategy: \n\n";
		cout << "1: First fit approach.\n" << "2: Best fit approach.\n\n";
		while (dec != 1 && dec != 2)//strategy
		{
			cin >> dec;
			if (dec == 1)
			{
				strategy = true;
				break;


			}
			else if (dec == 2)
			{
				break;
			}
		}
		cout << "\nEnter size of pool in KB: \n";
		cin >> dec;
		sizeOfmemory = dec;
		pool.insertAtStart(block(0, (sizeOfmemory * 1000)));//initialises pool

		cout << "\nWhich commands would you like to perform?";//menu

		while (run == true)
		{
			string id;
			int size;
			cout << "\n\n\n";
			cout << " 1: Add program to memory. \n 2: Delete program. \n 3: See current programs in RAM. \n 4: See pool of available memory in RAM. \n 5: Exit RAM menu. \n\n";
			cin >> dec;
			while (dec < 1 || dec > 5)
			{
				cout << "\nInvalid command. Please enter a number from options listed above.\n\n";
				cin >> dec;
			}
			switch (dec)
			{
			case 1:
			{

				cout << "\nEnter program id: ";
				cin >> id;
				cout << "\nEnter size of memory: ";
				cin >> size;
				if (GetMem(id, size))
				{
					cout << "\nMemory successfully allocated.\n";
				}
				else
				{
					cout << "\nMemory could not be allocated. Not enough free space in RAM.\n";
				}
				break;

			}
			case 2:
			{
				cout << "\nEnter program id: ";
				cin >> id;
				if (removeMem(id))
				{
					cout << "\nMemory successfully deallocated.\n";
				}
				else
				{
					cout << "\nMemory could not be deallocated. Program does not exist in RAM.\n";
				}
				break;

			}
			case 3:
			{
				cout << "\nPrograms currently occupying memory in RAM are: \n\n";
				progs.printprogs();
				break;
			}
			case 4:
			{
				cout << "\nPool of sorted free memory blocks in RAM: \n\n";
				pool.printblocks();
				break;
			}
			case 5:
			{
				cout << "\nThank you for using RAM. Goodbye. \n";
				run = false;
				break;
			}
			}
		}
	}

	~memorymanagementsystem()
	{

	}
};

void driver()//driver fucntion to create mms object and run it
{
	memorymanagementsystem ram;
	ram.run();
}

int main()
{
	driver();
}