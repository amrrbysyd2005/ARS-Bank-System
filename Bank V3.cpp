#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
using namespace std;

const string ClientsFileName = "Clients.txt";
const string dlim = "#//#";
const string bankName = "ARS";

enum class enUserRole
{
	Customer = 1,
	Employee = 2,
	Manager = 3
};




struct stClient {
	string AccountNumber;
	string PinCode;
	string FullName;
	string Phone;
	double Balance;
	enUserRole Role;
};

enum enMainMenu {
	Show_Client_List = 1, Add_New_Client = 2,
	Delete_Client = 3, Update_Client_Info = 4,
	Find_Client = 5, 
	Exit = 6
};

// ==========================================
//  HELPER DISPLAY & FORMATTING FUNCTIONS
// ==========================================

void PrintSeparator(const string& Style, int length) {
	for (int i = 0; i < length; i++) {
		cout << Style;
	}
}

void GeneralHeader(const string& head, const string& style = "=", int len = 80) {
	PrintSeparator(style, len);
	cout << "\n";

	int padding = (len - 2 - (int)head.length()) / 2;
	if (padding < 0) padding = 0;

	cout << "|";
	PrintSeparator(" ", padding);
	cout << head;
	PrintSeparator(" ", len - 2 - padding - (int)head.length());
	cout << "|\n";

	PrintSeparator(style, len);
	cout << "\n";
}

// ==========================================
//  INPUT & UTILITY FUNCTIONS
// ==========================================

static int ReadIntNumbers(const string& str) {
	int num = 0;
	while (true) {
		cout << str;
		cin >> num;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid Input!\n";
		}
		else if (cin.peek() != '\n' && cin.peek() != EOF) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid Input! (Please enter numbers only)\n";
		}
		else {
			return num;
		}
	}
}


static double ReadDoubleNumbers(const string& str) {
	double num = 0;
	while (true) {
		cout << str;
		cin >> num;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid Input!\n";
		}
		else if (cin.peek() != '\n' && cin.peek() != EOF) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid Input! (Please enter numbers only)\n";
		}
		else {
			return num;
		}
	}
}


static int ReadIntWithMaxAndMinLimit(const string& str, int Minlimit, int Maxlimit) {
	int num = 0;
	do {
		num = ReadIntNumbers(str);
		if (num <= Maxlimit && num >= Minlimit) return num;
		cout << "Please enter a value between " << Minlimit << " and " << Maxlimit << ".\n";
	} while (true);
}




vector<stClient>::iterator FindClientByAccountNumber(vector<stClient>& vClients, const string& accountNumber) {
	for (auto it = vClients.begin(); it != vClients.end(); it++) {
		if (it->AccountNumber == accountNumber) {
			return it;
		}
	}return vClients.end();
}
string ReadString(const string& str) {
	string S = "";

	cout << str;

	if (cin.fail()) {
		cin.clear();
	}

	getline(cin >> ws, S);

	return S;
}
bool CheckStringLength(const string& S, size_t length) {
	return (S.length() == length);
}

bool IsDigitsOnly(const string& S) {
	if (S.empty()) return false;
	return all_of(S.begin(), S.end(), [](unsigned char c) {
		return std::isdigit(c);
		});
}
string ReadClientAccountNumber() {
	string accountNumber = ReadString("\nEnter Account Number:\t");

	while (accountNumber.empty()) {
		cout << "[Error] Account number cannot be empty!\n";
		accountNumber = ReadString("Please enter a valid Account Number:\t");
	}

	for (char& c : accountNumber) {
		c = toupper(c);
	}

	return accountNumber;
}

string ReadPinCode() {
	string pinCode = ReadString("\nEnter PIN Code [4] digits:\t");

	while (!CheckStringLength(pinCode, 4) || !IsDigitsOnly(pinCode)) {
		cout << "[Error] PIN Code must be exactly 4 digits (numbers only)!\n";
		pinCode = ReadString("Please enter a valid [4] digit PIN Code:\t");
	}

	return pinCode;
}

string ReadFullName() {
	string name = ReadString("\nEnter The Full Name (First & Last name):\t");

	while (name.empty() || name.find(' ') == string::npos) {
		cout << "[Error] Please enter a valid full name (at least first and last name)!\n";
		name = ReadString("Enter The Full Name:\t");
	}

	return name;
}

string ReadPhoneNumber() {
	string PhoneNumber = ReadString("\nEnter The Phone Number [11] digits (e.g., 01xxxxxxxxx):\t");

	while (!CheckStringLength(PhoneNumber, 11) || !IsDigitsOnly(PhoneNumber) || PhoneNumber.substr(0, 2) != "01") {
		cout << "[Error] Phone Number must be exactly 11 digits and start with '01'!\n";
		PhoneNumber = ReadString("Please enter a valid Phone Number:\t");
	}

	return PhoneNumber;
}

double ReadBalance() {
	double Balance = ReadDoubleNumbers("\nEnter Account Balance:\t");
	while (Balance < 0) {
		cout << "[Error] Account balance cannot be negative!\n";
		Balance = ReadDoubleNumbers("Please enter a valid Account Balance (>= 0):\t");
	}
	return Balance;
}


void RoleMenu() {
	cout << "\nChoose The Role of added Person:\n";
	PrintSeparator("-", 40);
	cout << "\n";
	cout << "1. Manager\n"
		<< "2. Employee\n"
		<< "3. Customer\n";
	
	PrintSeparator("-", 40);
	cout << "\n";
}


enUserRole GetRole() {
	RoleMenu();
	
	return static_cast<enUserRole>(ReadIntWithMaxAndMinLimit("What Your Choice [1-3]?\t", 1, 3));
}


stClient GetClientInput(vector<stClient>& Clients, const string& currentAccountNumber) {
	stClient Client;
	string accountNumber = ReadClientAccountNumber();
	auto currentUserIt = FindClientByAccountNumber(Clients, currentAccountNumber);
	while (FindClientByAccountNumber(Clients, accountNumber) != Clients.end()) {
		cout << "Client with [" << accountNumber << "] already exists, enter another account number?\n ";
		accountNumber = ReadClientAccountNumber();

	}
	Client.AccountNumber = accountNumber;
	Client.PinCode = ReadPinCode();
	Client.FullName = ReadFullName();
	Client.Phone = ReadPhoneNumber();
	Client.Balance = ReadBalance();
	if (currentUserIt->Role == enUserRole::Manager) {
		Client.Role = GetRole();
	}
	else {
		Client.Role = enUserRole::Customer;
	}

	return Client;
}

// ==========================================
//  TABLE & CARD DISPLAY FUNCTIONS
// ==========================================

void PrintTableHeader() {
	PrintSeparator("-", 95);
	cout << "\n";
	cout << "| " << left << setw(18) << "Account Number"
		<< "| " << left << setw(12) << "Pin Code"
		<< "| " << left << setw(28) << "Client Name"
		<< "| " << left << setw(15) << "Phone"
		<< "| " << right << setw(12) << "Balance"
		<< "|\n";
	PrintSeparator("-", 95);
	cout << "\n";
}

void PrintClientRow(const stClient& Client) {
	cout << "| " << left << setw(18) << Client.AccountNumber
		<< "| " << left << setw(12) << Client.PinCode
		<< "| " << left << setw(28) << Client.FullName
		<< "| " << left << setw(15) << Client.Phone
		<< "| " << right << setw(12) << fixed << setprecision(2) << Client.Balance << "|\n";
}


void PrintAllClientsRows(const vector<stClient>& Clients) {
	for (const auto& client : Clients) {
		PrintClientRow(client);
	}
}

void PrintAllClients(const vector<stClient>& Clients) {
	system("cls");
	string title = "Client List (" + to_string(Clients.size()) + ") Client(s)";
	GeneralHeader(title, "=", 95);
	PrintTableHeader();
	PrintAllClientsRows(Clients);
	PrintSeparator("-", 95);
	cout << "\n";
}

void PrintClientCard(const stClient& Client) {
	cout << "\n========================================\n";
	cout << "          CLIENT DETAILS CARD           \n";
	cout << "========================================\n";
	cout << " Account Number : " << left << Client.AccountNumber << "\n";
	cout << " Pin Code       : " << left << Client.PinCode << "\n";
	cout << " Name           : " << left << Client.FullName << "\n";
	cout << " Phone          : " << left << Client.Phone << "\n";
	cout << " Balance        : $" << left << fixed << setprecision(2) << Client.Balance << "\n";
	cout << "========================================\n";
}

// ==========================================
//  FILE & CONVERSION LOGIC
// ==========================================

string ConvertClientToLine(const stClient& Client, const string& dlim = "#//#") {
	return Client.AccountNumber + dlim +
		Client.PinCode + dlim +
		Client.FullName + dlim +
		Client.Phone + dlim +
		to_string(Client.Balance)+ dlim+
		(Client.Role==enUserRole::Manager?"M": Client.Role == enUserRole::Employee ? "E":"C");
}



vector<string> SplitLineToVector(string line, const string& dlim) {
	size_t pos = 0;
	vector<string> vRecord;
	string word;
	while ((pos = line.find(dlim)) != string::npos) {
		word = line.substr(0, pos);
		if (!word.empty()) vRecord.push_back(word);
		line.erase(0, pos + dlim.length());
	}
	if (!line.empty()) vRecord.push_back(line);
	return vRecord;
}

stClient ParseLineToClient(const vector<string>& vRecord) {
	stClient Client;
	if (vRecord.size() >= 6) {
		Client.AccountNumber = vRecord.at(0);
		Client.PinCode = vRecord.at(1);
		Client.FullName = vRecord.at(2);
		Client.Phone = vRecord.at(3);
		Client.Balance = stod(vRecord.at(4));
		Client.Role = (vRecord.at(5) == "M" ? enUserRole::Manager : vRecord.at(5) == "C" ? enUserRole::Customer : enUserRole::Employee);
	}
	return Client;
}

void LoadClientsFromFile(vector<stClient>& Clients, const string& FilePath) {
	fstream Myfile(FilePath, ios::in);
	if (!Myfile.is_open()) 
	{
		ofstream CreateFile(FilePath);
		if (CreateFile.is_open()) 
		{
			CreateFile.close();
		}
		else 
		{
			cout << "[Error] Unable to create file: " << FilePath << "\n";
			return;
		}
	} 
	else
	{
		string line;
		while (getline(Myfile, line)) {
			if (!line.empty()) {
				Clients.push_back(ParseLineToClient(SplitLineToVector(line, dlim)));
			}
		}
		Myfile.close();
	}
}


bool SaveClientsToFile(const vector<stClient>& Clients) {
	fstream MyFile(ClientsFileName, ios::out);
	if (MyFile.is_open()) {
		for (const stClient& Client : Clients) {
			MyFile << ConvertClientToLine(Client, dlim) << "\n";
		}
		MyFile.close();
		return true;
	}
	else {
		cout << "[Error] Unable to open file for writing!\n";
		return false;
	}
}



//رسالة الترحيب
void printWelcomeScreen(const string& bankName) {
	system("cls");

	cout << "\n\n\n\n\n\n";

	string indent = "\t\t\t";

	cout << indent; PrintSeparator("=", 60); cout << "\n";

	int headLen = (int)("WELCOME TO " + bankName + " BANK").length();
	int pad1 = (60 - 2 - headLen) / 2;
	cout << indent << "|";
	PrintSeparator(" ", pad1);
	cout << "WELCOME TO " << bankName << " BANK";
	PrintSeparator(" ", 60 - 2 - pad1 - headLen);
	cout << "|\n";

	cout << indent; PrintSeparator("=", 60); cout << "\n";

	string subTitle = "Secure & Reliable Banking Services";
	int subLen = (int)subTitle.length();
	int pad2 = (60 - 2 - subLen) / 2;
	cout << indent << "|";
	PrintSeparator(" ", pad2);
	cout << subTitle;
	PrintSeparator(" ", 60 - 2 - pad2 - subLen);
	cout << "|\n";

	cout << indent; PrintSeparator("=", 60); cout << "\n\n";

	cout << indent << "---> Press ANY KEY to start the system... ";

	system("pause>0");
	system("cls");
}



enum class enSaveChoice { Save = 1, SaveLater = 2, Cancel = 3 };

enSaveChoice confirmSaveOrCancel() {
	cout << "\n--------------------------------------------------\n";
	cout << "Would you like to:\n";
	cout << " [1] Change and Save to File NOW\n";
	cout << " [2] Change and Save to File later\n";
	cout << " [3] Cancel / Discard changes\n";

	return static_cast<enSaveChoice>(ReadIntWithMaxAndMinLimit("Enter choice (1-3):\t", 1, 3));

}

void AddClient(vector<stClient>& Clients, const string& currentAccountNumber, bool &isDataChanged) {
	stClient newClient = GetClientInput(Clients, currentAccountNumber);

	enSaveChoice SaveChoice =  confirmSaveOrCancel();
	switch (SaveChoice) {
	case enSaveChoice::Save:
	{
		Clients.push_back(newClient);
		if (SaveClientsToFile(Clients)) {
			cout << "\n[SUCCESS] Client added successfully to memory and file!\n";
		}
	}break;
	case enSaveChoice::SaveLater: {
		Clients.push_back(newClient);
		cout << "\n[SUCCESS] Client added successfully to memory!\n";
		isDataChanged = true;
	}
		break;
	case enSaveChoice::Cancel:
		cout << "\n[SYSTEM] Operation cancelled. Client was not saved.\n";
		break;
	}

}

char readYesOrNo(const string& str) {
	char ch;
	do {
		cout << str;
		cin >> ch;

		if (cin.fail()) {
			cin.clear();
		}
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		ch = toupper(ch);

		if (ch != 'Y' && ch != 'N') {
			cout << "[Error] Invalid input! Please enter 'Y' or 'N'.\n\n";
		}

	} while (ch != 'Y' && ch != 'N');

	return ch;
}

//  شاشة إضافة عميل
void ShowAddClientScreen(vector<stClient>& vClients, const string& currentAccountNumber, bool &isDataChanged) {
	char ch;
	do {
		system("cls");
		GeneralHeader("<< Addition Page >>", "=", 80);

		AddClient(vClients, currentAccountNumber, isDataChanged);

		cout << "\n--------------------------------------------------\n";

		ch = readYesOrNo("Do you want to add another client? [Y/N]: ");

	} while (ch == 'Y');
}



enum class DeletionChoice {
	SaveNow = 1,
	SaveLater = 2,
	Cancel = 3
};

DeletionChoice getDeletionConfirmation() {
	int choice;

		cout << "\n--------------------------------------------------\n";
		cout << "[WARNING] You are about to delete this client.\n";
		cout << "Select how to process this action:\n";
		cout << " [1] Delete and Save to file NOW\n";
		cout << " [2] Delete for now (Save LATER)\n";
		cout << " [3] Cancel deletion\n";
		cout << "Enter choice (1-3): ";
		choice = ReadIntWithMaxAndMinLimit("Enter choice (1-3):\t", 1, 3);
		switch (choice) {
		case 1:
			return DeletionChoice::SaveNow;
		case 2:
			return DeletionChoice::SaveLater;
		case 3:
			return DeletionChoice::Cancel;
		}
		return DeletionChoice::SaveNow;
}


void commitClientDeletion(vector<stClient>& vClients, vector<stClient>::iterator it) {
	vClients.erase(it);
	SaveClientsToFile(vClients);
	cout << "\n[SUCCESS] Client deleted and file updated successfully!\n";
}

void deleteClientFromMemory(vector<stClient>& vClients, vector<stClient>::iterator it) {
	vClients.erase(it);
	cout << "\n[SYSTEM] Client removed from memory.\n";
}



vector<stClient>::iterator getExistingClientOrNull(vector<stClient>& vClients) {
	string accountNumber = ReadClientAccountNumber();
	auto it = FindClientByAccountNumber(vClients, accountNumber);

	if (it == vClients.end()) {
		cout << "\n[Error] Account number (" << accountNumber << ") is not registered.\n";
	}

	return it;
}


void DeleteClient(vector<stClient>& vClients, bool& isDataChanged) {

	if (vClients.empty()) {
		cout << "\n[Info] Client list is currently empty.\n";
		return;
	}

	auto it = getExistingClientOrNull(vClients);

	if (it == vClients.end()) return;

	cout << "\nClient Details:\n";
	PrintClientCard(*it);

	switch (getDeletionConfirmation()) {
	case DeletionChoice::SaveNow:
		commitClientDeletion(vClients, it);
		break;

	case DeletionChoice::SaveLater:
		isDataChanged = true;
		deleteClientFromMemory(vClients, it);
		break;

	case DeletionChoice::Cancel:
		cout << "\n[SYSTEM] Deletion cancelled.\n";
		break;
	}
}


void ShowDeleteClientScreen(vector<stClient>& vClients, bool& isDataChanged) {
	char ch;
	do {
		system("cls");
		GeneralHeader("<<Deletion Page>>", "=", 80);
		DeleteClient(vClients, isDataChanged);

		cout << "\n--------------------------------------------------\n";
		ch = readYesOrNo("Do you want to delete another client? [Y/N]: ");

	} while (ch == 'Y');

}

void searchClient(vector<stClient>& vClients) {

	if (vClients.empty()) {
		cout << "\n[Info] Client list is currently empty.\n";
		return;
	}

	auto it = getExistingClientOrNull(vClients);

	if (it == vClients.end()) return;

	PrintClientCard(*it);
}


void ShowSearchClientScreen(vector<stClient>& vClients) {
	char ch;
	do {
		system("cls");
		GeneralHeader("<<Search Page>>", "=", 80);

		searchClient(vClients);

		cout << "\n--------------------------------------------------\n";
		ch = readYesOrNo("Do you want to search another client? [Y/N]: ");

	} while (ch == 'Y');
}


int ReadUpdateMenuChoice() {
	cout << "\nWhat do you want to update?\n";
	cout << "[1] Pin Code\n";
	cout << "[2] Full Name\n";     
	cout << "[3] Phone Number\n";  
	cout << "[4] Balance\n";       
	return ReadIntWithMaxAndMinLimit("Enter Your Choice [1-4]: ", 1, 4);
}

enum class enUpdateMenu {
	PinCode = 1,
	Name = 2,
	PhoneNumber = 3,
	Balance = 4
};

void updateClientPINCode(vector<stClient>::iterator it) {
	it->PinCode = ReadPinCode();
	cout << "\n[SUCCESS] The PIN code has been successfully changed.\n";
}

void updateClientFullName(vector<stClient>::iterator it) {
	it->FullName = ReadFullName();
	cout << "\n[SUCCESS] The full name has been successfully changed.\n";
}

void updateClientPhoneNumber(vector<stClient>::iterator it) {
	it->Phone = ReadPhoneNumber();
	cout << "\n[SUCCESS] The phone number has been successfully changed.\n";
}

void updateClientBalance(vector<stClient>::iterator it) {
	it->Balance = ReadBalance();
	cout << "\n[SUCCESS] The balance has been successfully changed.\n";
}

void updateClient(vector<stClient>& vClients, const string& currentAccountNumber, bool& isDataChanged) {

	if (vClients.empty()) {
		cout << "\n[Info] Client list is currently empty.\n";
		return;
	}
	auto currentUserIt = FindClientByAccountNumber(vClients, currentAccountNumber);
	auto it = getExistingClientOrNull(vClients);

	if (it == vClients.end()) return;

	if (currentUserIt->Role == enUserRole::Manager) {
		cout << "\n--- Current Client Details ---\n";
		PrintClientCard(*it);
	}

	stClient originalClientData = *it;

	enUpdateMenu choice = (enUpdateMenu)ReadUpdateMenuChoice();

	switch (choice) {
	case enUpdateMenu::PinCode:
		updateClientPINCode(it);
		break;
	case enUpdateMenu::Name:
		updateClientFullName(it);
		break;
	case enUpdateMenu::PhoneNumber:
		updateClientPhoneNumber(it);
		break;
	case enUpdateMenu::Balance:
		if (currentUserIt->Role == enUserRole::Manager) {
			updateClientBalance(it);
		}
		else {
			cout << "Sorry, you do not have sufficient permissions to Update balance for this account.\n";
		}
		break;
	}
	if (currentUserIt->Role == enUserRole::Manager) {
		cout << "\n--- Updated Client Details ---\n";
		PrintClientCard(*it);
	}

	switch (confirmSaveOrCancel()) {
	case enSaveChoice::Save:
		SaveClientsToFile(vClients);
		cout << "\n[SUCCESS] Changes updated in memory and saved to file NOW!\n";
		break;

	case enSaveChoice::SaveLater: {
		cout << "\n[INFO] Changes kept in memory. (Will be saved to file upon exiting/saving).\n";
		isDataChanged = true;
	}
		break;

	case enSaveChoice::Cancel:
		*it = originalClientData;
		cout << "\n[CANCELLED] Changes discarded. Client data restored.\n";
		break;
	}
}

void ShowUpdateClientScreen(vector<stClient>& vClients, const string& currentAccountNumber, bool& isDataChanged) {

	

	char ch;
	do {
		system("cls");
		GeneralHeader("<< Update Page >>", "=", 80);

		updateClient(vClients, currentAccountNumber, isDataChanged);

		cout << "\n--------------------------------------------------\n";
		ch = readYesOrNo("Do you want to update data for another client? [Y/N]: ");

	} while (ch == 'Y');
}



void HandleSystemExit(vector<stClient>& vClients, bool& isDataChanged) {
	if (isDataChanged) {
		cout << "\n--------------------------------------------------\n";
		cout << "[WARNING] You have unsaved changes in memory!\n";

		char choice = readYesOrNo("Do you want to save changes to file before exiting? [Y/N]: ");

		if (choice == 'Y') {
			SaveClientsToFile(vClients);
			isDataChanged = false;
			cout << "[SUCCESS] All changes saved successfully.\n";
		}
		else {
			cout << "[INFO] Exiting without saving changes.\n";
		}
	}

}


void DepositLogic(vector<stClient>::iterator it) {
	double DepositAmount = ReadDoubleNumbers("Please enter deposit amount:\t");
	while (DepositAmount < 0) {
		cout << "[Error]  deposit amount cannot be negative!\n";
		DepositAmount = ReadDoubleNumbers("Please enter a valid deposit amount (> 0):\t");
	}
	it->Balance += DepositAmount;
}


void ShowDepositScreen(vector<stClient>::iterator it) {
	system("cls");

	GeneralHeader("<<Deposit Screen>>", "-", 40);
	cout << "\n--- Current Client Details ---\n";
	PrintClientCard(*it);

	DepositLogic(it);

	cout << "\n--- Current Client Details ---\n";
	PrintClientCard(*it);


}



void WithdrawLogic(vector<stClient>::iterator it) {

	double WithDraw = ReadDoubleNumbers("Please enter withdraw amount:\t");
	while (WithDraw < 0||WithDraw>it->Balance) {
		if (WithDraw <= 0) {
			cout << "[Error]  withdraw amount cannot be negative!\n";
		}if (WithDraw > it->Balance) {
			cout << "You cannot withdraw $" << WithDraw << "; your limit is $" << it->Balance << ".\n";
		}
		WithDraw = ReadDoubleNumbers("Please enter a valid withdraw amount (> 0):\t");

	}
	it->Balance -= WithDraw;
}

void ShowWithDrawScreen(vector<stClient>::iterator it) {
	system("cls");

	GeneralHeader("<<Withdraw Screen>>", "-", 40);
	cout << "\n--- Current Client Details ---\n";
	PrintClientCard(*it);

	WithdrawLogic(it);

	cout << "\n--- Current Client Details ---\n";
	PrintClientCard(*it);


}





void ShowTotalBalanceScreen(vector<stClient>::iterator it) {
	system("cls");

	GeneralHeader("<<Balance Screen>>", "-", 40);
	cout << "\n Account Number:\t" << it->AccountNumber;
	cout << "\n Full Name:\t" << it->FullName;
	cout << "\n Total Balance:\t" << it->Balance << endl;
}



enum class enTransactionProcess {
	Deposit = 1,
	Withdraw = 2,
	Total_Balance = 3,
	Exit=4
};



void ShowTransactionMenu() {
	system("cls");
	GeneralHeader("Transaction Menu Screen", "=", 40);
	cout << "  [1] Deposit.\n";
	cout << "  [2] Withdraw.\n";
	cout << "  [3] Total Balance.\n";
	cout << "  [4] Exit.\n";
	PrintSeparator("=", 40);
	cout << "\n";
}


void GoBackToMainMenue() {
	cout << "\n\nPress any key to go back to Main Menu...";
	system("pause>0");
}


void ShowEndScreen() {
	system("cls");
	GeneralHeader("Program Ended - Thank You :-)", "=", 50);
}




bool PerformeTransactionProcess(vector <stClient>::iterator it) {
	enTransactionProcess Process = (enTransactionProcess)ReadIntWithMaxAndMinLimit("Choose what do you want to do? [1 to 4]: ", 1, 4);

		switch (Process) {
		case enTransactionProcess::Deposit:
			ShowDepositScreen(it);
			GoBackToMainMenue();
			return false;
		case enTransactionProcess::Withdraw:
			ShowWithDrawScreen(it);
			GoBackToMainMenue();
			return false;
		case enTransactionProcess::Total_Balance:
			ShowTotalBalanceScreen(it);
			GoBackToMainMenue();
			return false;
		case enTransactionProcess::Exit:
			return true;
	}
}

void ShowTransactionOnScreen(vector<stClient>& vClients, vector <stClient>::iterator it) {
	while (true) {
		ShowTransactionMenu();
		bool Exit = PerformeTransactionProcess(it);
		if (Exit) {
			SaveClientsToFile(vClients);
			return;
		}
	}
}





void ShowMainMenue() {
	system("cls");
	GeneralHeader("Main Menu Screen", "=", 40);
	cout << "  [1] Show Client List.\n";
	cout << "  [2] Add New Client.\n";
	cout << "  [3] Delete Client.\n";
	cout << "  [4] Update Client Info.\n";
	cout << "  [5] Find Client.\n";
	cout << "  [6] Exit.\n";
	PrintSeparator("=", 40);
	cout << "\n";
}



bool PerformMainMenuOption(vector<stClient>& Clients, const string& currentAccountNumber, bool &isDataChanged) {

	    auto currentUserIt = FindClientByAccountNumber(Clients, currentAccountNumber);
		enMainMenu choice = (enMainMenu)ReadIntWithMaxAndMinLimit("Choose what do you want to do? [1 to 6]: ", 1, 6);;
		switch (choice) {
		case enMainMenu::Show_Client_List:

			if (currentUserIt->Role != enUserRole::Manager) {
				cout << "Sorry, you do not have sufficient permissions to view accounts..\n";
			}
			else {
				PrintAllClients(Clients);
			}
			GoBackToMainMenue();
			return false;
		case enMainMenu::Add_New_Client:
			ShowAddClientScreen(Clients, currentAccountNumber, isDataChanged);
			GoBackToMainMenue();
			return false;

		case enMainMenu::Delete_Client:
			if (currentUserIt->Role != enUserRole::Manager) {
				cout << "Sorry, you do not have sufficient permissions to delete the account.\n";
			}
			else {
				ShowDeleteClientScreen(Clients, isDataChanged);
			}
			GoBackToMainMenue();
			return false;
		case enMainMenu::Update_Client_Info:
			ShowUpdateClientScreen(Clients, currentAccountNumber, isDataChanged);
			GoBackToMainMenue();
			return false;
		case enMainMenu::Find_Client:
			ShowSearchClientScreen(Clients);
			GoBackToMainMenue();
			return false;
		case enMainMenu::Exit:
			HandleSystemExit(Clients, isDataChanged);
			return true;
		}
		return false;
}

void ShowSensitiveOperationsOnScreen(vector<stClient>& Clients, vector<stClient>::iterator currentUserIt) {
	string currentAccountNumber = currentUserIt->AccountNumber;
	bool isDataChanged = false;
	while (true) {
		ShowMainMenue();
		
		bool Exit = PerformMainMenuOption(Clients, currentAccountNumber, isDataChanged);
		if (Exit) {
			return;
		}
	}
}






void LoginPage(vector<stClient>& vClients) {

	GeneralHeader("Login page", "=", 40);

	auto it = getExistingClientOrNull(vClients);

	while (it == vClients.end()) {
		char ch = readYesOrNo("Do you want to cancel? [Y] for Yes, [N] for No:\t");
		if (ch == 'Y') return;
		it = getExistingClientOrNull(vClients);
	}

	const int MAX_ATTEMPTS = 3;
	int remainingAttempts = MAX_ATTEMPTS;
	string pinCode;

	while (remainingAttempts > 0) {
		pinCode = ReadPinCode();

		if (pinCode == it->PinCode) {
			if (it->Role == enUserRole::Customer) {
				ShowTransactionOnScreen(vClients, it);
			}
			else {
				ShowSensitiveOperationsOnScreen(vClients, it);
			}
			return; 
		}

		--remainingAttempts;

		if (remainingAttempts > 0) {
			cout << "\n[Error] Invalid PIN code! You have "
				<< remainingAttempts << " attempt(s) left.\n\n";
		}
	}

	cout << "\n[Access Denied] Too many failed attempts! Returning to main screen...\n";
}


void StartBankSystem(vector<stClient>& vClients) {
	printWelcomeScreen(bankName);
	LoginPage(vClients);
	ShowEndScreen();
}


int main()
{
	vector<stClient> vClients; 
	LoadClientsFromFile(vClients, ClientsFileName);
	StartBankSystem(vClients);
}
