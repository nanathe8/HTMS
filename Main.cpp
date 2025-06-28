#include <iostream>
#include <string>
#include <conio.h>
#include <iomanip>
#include <mysql.h> //libmysql
using namespace std;

//Admin Function
void AdminLogin();
void AdminMenu();
void ChangeAdminPassword();
void AdminProfile();
void AdminViewProfile();
void ViewRooms();
void AddRoom();
void DeleteRoom();
void Blocks();
void ViewBlocks();
void AddBlock();
void DeleteBlock();
void RoomPageAdmin();
void ViewHostelReport_();
void StudentDetails();
void SearchStudent();
void AddStudent();
void AdminStudentPayment();

//Student Function
void StudentLogin();
void StudentMenu();
void ProfileForStudent();
void RegisterRoom();
void Payment();
void HostelDetail();
void HostelReportMenu();
void StudentProfile();
void ChangeStudentPassword();
void Students();
void StudentHostelReport();
void ViewHostelReport();

//Registration Function
string getGenderFromIC(const string& icNumber);

int qstate;
MYSQL* conn;
MYSQL_ROW row;
MYSQL_RES* res;

string UserID;

// Global variable to store StudentID and AdminID
string GlobalStudentID = "";
string GlobalAdminID = "";

class db_response
{
public:
    static void ConnectionFunction()
    {
        conn = mysql_init(0);
        if (conn)
            cout << "Database Connected" << endl;
        else
            cout << "Failed To Connect!" << endl;

        conn = mysql_real_connect(conn, "localhost", "root", "", "dbhms_2223", 3306, NULL, 0);
        if (conn)
            cout << "Database Connected To MySql" << endl;
        else
            cout << "Failed To Connect!" << endl;
    }
};

int main()
{

    system("cls");
    system("title My Project");
    db_response::ConnectionFunction();

    int MainMenu;

    // Admin login, Student login, Student registration
    cout << "\n" << "---------------------------------------------" << endl;
    cout <<"      WELCOME TO UTeM ONLINE HOSTEL!" << endl;
    cout << "---------------------------------------------" << endl;
    cout << "\n\t[1]- Admin Login" << endl;
    cout << "\t[2]- Student Login" << endl;
    cout << "\t[3]- Exit" << endl;
    cout << "\nChoose: ";
    cin >> MainMenu;

    switch (MainMenu)
    {
    case 1:
        AdminLogin();
        break;
    case 2:
        StudentLogin();
        break;
    case 3:
        cout << "Program Exiting..." << endl;
        exit(0);
    default:
        cout << "Please choose between 1 - 3. Press Enter To Continue...";
        _getch(); //get char - pause console
        system("cls");
        main();
        break;
    }

    return 0;
}

void RegisterRoom()
{
    system("cls");
    string StudentName, StudentID, RoomID, RegisterDate, payment, ICNumber, PaymentID;
    char option;

    //cout << "\t\t--- Student Register Room ---" << endl;
    cout << "\n" << "---------------------------------------------" << endl;
    cout << "         REGISTRATION ROOM" << endl;
    cout << "---------------------------------------------" << endl;
    cout << "\n Welcome " << GlobalStudentID << "!" << "   You Need To Register Your Room First." << endl;
    cin.ignore(1, '\n');
    cout << "\nEnter matric number: ";
    getline(cin, GlobalStudentID);

    string checkUserQuery = "SELECT StudentID FROM student WHERE StudentID = '" + GlobalStudentID + "'";
    const char* cu = checkUserQuery.c_str();
    qstate = mysql_query(conn, cu);

    if (qstate != 0) {
        cout << "Query Execution Problem: " << mysql_error(conn) << endl;
        return;
    }

    res = mysql_store_result(conn);
    if (res->row_count != 1) {
        cout << "\nYour StudentID is not in the database list." << endl;
        return;
    }

    // Get ICNumber from the database using StudentID
    string getICNumberQuery = "SELECT ICNumber FROM student WHERE StudentID = '" + GlobalStudentID + "'";
    const char* getICNumberQ = getICNumberQuery.c_str();
    qstate = mysql_query(conn, getICNumberQ);

    if (qstate != 0) {
        cout << "Error getting IC number: " << mysql_error(conn) << endl;
        return;
    }

    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);

    if (row == nullptr) {
        cout << "IC number not found." << endl;
        return;
    }

    string icNumber = row[0];

    // Determine gender using the getGenderFromIC function
    string gender = getGenderFromIC(icNumber);

    string checkUserQuery2 = "SELECT r.StudentID, s.StudentStatus FROM registration_room r JOIN student s ON r.StudentID = s.StudentID WHERE r.studentID = '" + GlobalStudentID + "' AND s.StudentStatus = 'Approve'";
    const char* cu2 = checkUserQuery2.c_str();
    qstate = mysql_query(conn, cu2);

    if (qstate != 0) {
        cout << "Error checking user: " << mysql_error(conn) << endl;
        return;
    }

    res = mysql_store_result(conn);
    if (res->row_count == 1) {
        cout << "\n\nUsername is already registered." << endl;
        cout << "\nPress Enter To Back...";
        _getch();
        main();
        return;
    }

    // Assuming 'gender' is 'male' or 'female' based on the user's input
    string genderCondition = (gender == "male") ? "b.BlockGender = 'Male'" : "b.BlockGender = 'Female'";

    // Modify the query to include the gender
    string roomQuery = "SELECT r.RoomID, r.RoomStatus, r.RoomLevel, b.BlockName, b.BlockGender FROM room r INNER JOIN block b ON r.BlockID = b.BlockID WHERE r.RoomStatus = 'Available' AND " + genderCondition;
  
    qstate = mysql_query(conn, roomQuery.c_str());

    if (qstate != 0) {
        cout << "\n\nQuery Execution Problem!" << mysql_errno(conn) << endl;
        return;
    }

    cout << "\n\n  Room Number" << "\t  Room Status" << "\t  Room Level" << "\t  Block Name" << endl;
    res = mysql_store_result(conn);
    while (row = mysql_fetch_row(res)) {
        cout << "\n" << row[0] << "\t  " << row[1] << "\t        " << row[2] << "\t\t" << row[3] << endl;
    }

    cout << "Press Enter to Continue...";
    cin.ignore(1, '\n');
    cout << "\nEnter room number (eg: SQ-Q-1-01-A(1)): " << endl;
    getline(cin, RoomID);
    cout << "Enter register date (yyyy-mm-dd): " << endl;
    getline(cin, RegisterDate);

    // Insert registration details
    string insertCustomerQuery = "INSERT INTO registration_room (StudentID, RoomID, RegisterDate) VALUES ('" + GlobalStudentID + "', '" + RoomID + "', '" + RegisterDate + "')";
    const char* insertQuery = insertCustomerQuery.c_str();
    qstate = mysql_query(conn, insertQuery);

    if (qstate != 0) {
        cout << "Error inserting registration details: " << mysql_error(conn) << endl;
        return;
    }

    // Update room status
    string updateRoom_query = "UPDATE room SET RoomStatus = 'Not Available' WHERE RoomID = '" + RoomID + "'";
    const char* updateRoomQuery = updateRoom_query.c_str();
    qstate = mysql_query(conn, updateRoomQuery);

    if (qstate != 0) {
        cout << "Error updating room status: " << mysql_error(conn) << endl;
        return;
    }

    // Update student status
    string updateStudent_query = "UPDATE student SET StudentStatus = 'Approve' WHERE StudentID = '" + GlobalStudentID + "'";
    const char* updateStudentQuery = updateStudent_query.c_str();
    qstate = mysql_query(conn, updateStudentQuery);

    if (qstate != 0) {
        cout << "Error updating student status: " << mysql_error(conn) << endl;
        return;
    }

    // Insert new payment record
    string insertPaymentQuery1 = "INSERT INTO payment (PaymentID, TotalAmount, PaymentMethod, BankName, PaymentStatus, PaymentDate, StudentID, ) VALUES ('', 'RM540', NULL, NULL, 'Not Paid', NULL, '" + GlobalStudentID + "')";
    const char* insertQuery1 = insertPaymentQuery1.c_str();
    qstate = mysql_query(conn, insertQuery1);


    if (qstate != 0) {
        cout << "Error updating payment StudentID: " << mysql_error(conn) << endl;
        return;
    }
    // Successful registration
    system("cls");
    cout << endl << "You room have been registered. " << endl;
    
    cout << "\nYour Hostel Cost Amount is : RM 540" << endl;
    cout << "Want to make payment now? (y/n): ";
    cin >> option;
    if (option == 'y' || option == 'Y')
    {
        Payment();
    }
    else
    {
        main();
    }
        _getch();
        StudentMenu();
    cout << "Press Enter to Continue...";
    _getch();
    main();
}

string getGenderFromIC(const string& icNumber) {
    // Check if the IC number is valid
    if (icNumber.length() != 12) {
        cout << "Invalid IC number." << endl;
        return "Unknown";
    }

    // Extract the seventh digit
    char genderDigit = icNumber[6];

    // Check if it's odd or even to determine gender
    return (genderDigit % 2 == 0) ? "Female" : "Male";
}

void StudentLogin()
{
    system("cls");
    string StudentID;
    string Password;

    cout << "----------Student Login-----------" << endl;
    cout << "\nMatric Number : ";
    cin >> StudentID;
    cout << "Password : ";
    char ch;
    while ((ch = _getch()) != 13)
    {
        Password += ch;
        cout << '*';
    }

    //Check the entered Matric Number and Password in the database
    string checkUser_query = "SELECT StudentID FROM student WHERE StudentID = '" + StudentID + "' and Password = '" + Password + "'";
    const char* cu = checkUser_query.c_str();
    qstate = mysql_query(conn, cu);

    if (!qstate)
    {
        res = mysql_store_result(conn);
        if (res->row_count == 1)
        {
            while (row = mysql_fetch_row(res))
                GlobalStudentID = row[0]; // Store the global StudentID for future use

            string checkUser_query = "select StudentID from registration_room where StudentID = '" + StudentID + "'";
            const char* cu = checkUser_query.c_str();
            qstate = mysql_query(conn, cu);

            if (!qstate) {
                res = mysql_store_result(conn);

                // Check if the StudentID is found in the registration_room table
                if (res->row_count > 0) {
                    // StudentID is in the registration_room, proceed to the StudentMenu
                    cout << "StudentID is registered. Proceeding to Student Menu..." << endl;
                    StudentMenu();
                }
                else {
                    // StudentID is not in the registration_room
                    cout << "StudentID is not registered. Please register a room first." << endl;
                    RegisterRoom();  // You can call your function to register a room here
                }
            }
            else {
                cout << "\nQuery Execution Problem!" << mysql_errno(conn) << endl;
            }
        }
        else
        {
            char c;
            cout << "\nInvalid username or password. Want to try again? (y/n): ";
            cin >> c;
            if (c == 'y' || c == 'Y')
                StudentLogin();
            else
                main();
        }
    }
    else
        cout << "\nQuery Execution Problem!" << mysql_errno(conn) << endl;
}
//int loginAttempts = 3; // Maximum number of login attempts
//
//void StudentLogin()
//{
//    system("cls");
//    string StudentID;
//    string Password;
//
//    cout << "----------Login Page-----------" << endl;
//
//    while (loginAttempts > 0) {
//        cout << "\nMatric Number : ";
//        cin >> StudentID;
//        cout << "Password : ";
//
//        char ch;
//        while ((ch = _getch()) != 13)
//        {
//            Password += ch;
//            cout << '*';
//        }
//
//        // Check the entered Matric Number and Password in the database
//        string checkUser_query = "SELECT StudentID FROM student WHERE StudentID = '" + StudentID + "' and Password = '" + Password + "'";
//        const char* cu = checkUser_query.c_str();
//        qstate = mysql_query(conn, cu);
//
//        if (!qstate)
//        {
//            res = mysql_store_result(conn);
//            if (res->row_count == 1)
//            {
//                // Successful login
//                while (row = mysql_fetch_row(res))
//                    GlobalStudentID = row[0]; // Store the global StudentID for future use
//
//                string checkUser_query = "select StudentID from registration_room where StudentID = '" + StudentID + "'";
//                const char* cu = checkUser_query.c_str();
//                qstate = mysql_query(conn, cu);
//
//                if (!qstate) {
//                    res = mysql_store_result(conn);
//
//                    // Check if the StudentID is found in the registration_room table
//                    if (res->row_count > 0) {
//                        // StudentID is in the registration_room, proceed to the StudentMenu
//                        cout << "\nStudentID is registered. Proceeding to Student Menu..." << endl;
//                        StudentMenu();
//                        return;
//                    }
//                    else {
//                        // StudentID is not in the registration_room
//                        cout << "\nStudentID is not registered. Please register a room first." << endl;
//                        RegisterRoom();  // You can call your function to register a room here
//                        return;
//                    }
//                }
//                else {
//                    cout << "\nQuery Execution Problem!" << mysql_errno(conn) << endl;
//                }
//            }
//            else
//            {
//                cout << "\nInvalid username or password. ";
//                loginAttempts--;
//
//                if (loginAttempts > 0) {
//                    cout << "You have " << loginAttempts << " attempts left." << endl;
//                }
//                else {
//                    cout << "No more attempts left. Exiting program." << endl;
//                    exit(0); // You may want to handle this differently based on your application's flow
//                }
//            }
//        }
//        else
//        {
//            cout << "\nQuery Execution Problem!" << mysql_errno(conn) << endl;
//            // You may want to handle this differently based on your application's flow
//        }
//    }
//}

void StudentMenu()
{
    system("cls");
    int studentmenu;

    cout << "\t---------Menu----------" << endl;
    cout << "\n\t[1]- Hostel Details" << endl;
    cout << "\t[2]- Hostel Report" << endl;
    cout << "\t[3]- Payment" << endl;
    cout << "\t[4]- Profile" << endl;
    cout << "\t[5]- Logout" << endl;
    cout << "\nChoose: ";
    cin >> studentmenu;

    switch (studentmenu)
    {
        case 1:
            HostelDetail();
            break;
        case 2:
            HostelReportMenu();
            break;
        case 3:
            Payment();
            break;
        case 4:
            StudentProfile();
            break;
    case 5:
        main();
        break;
    }
}

void HostelDetail()
{
    system("cls"); //  clear the screen
    cout << "\t------My Hostel Informations------" << endl;

    // SQL query to retrieve hostel details for the current student
        string query = "SELECT b.BlockName, b.BlockGender, rr.RoomID, rr.RegisterDate, p.TotalAmount, p.PaymentMethod, p.BankName, p.PaymentStatus, p.PaymentDate "
        "FROM registration_room rr "
        "JOIN room r ON rr.RoomID = r.RoomID "
        "JOIN block b ON r.BlockID = b.BlockID "
        "JOIN student s ON rr.StudentID = s.StudentID "
        "LEFT JOIN payment p ON rr.StudentID = p.StudentID "
        "WHERE s.StudentID = '" + GlobalStudentID + "'";  // Include WHERE clause to filter by StudentID

    // Execute the SQL query
    qstate = mysql_query(conn, query.c_str());

    if (qstate != 0)
    {
        cerr << "Query execution error: " << mysql_error(conn) << endl;
        return;
    }
    // Store the result of the query
    res = mysql_store_result(conn);

    if (res == nullptr)
    {
        cerr << "Error fetching result: " << mysql_error(conn) << endl;
        return;
    }

    // Fetch and print the results
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res)) != nullptr) 
    {
        cout << setw(15) << "\n\tBlock Name: " << row[0] << endl;
        cout << setw(15) << "\tBlock Gender: " << row[1] << endl;
        cout << setw(15) << "\tRoom Number: " << row[2] << endl;
        cout << setw(15) << "\tRegistration Date: " << row[3] << endl;
        
        // Check if the user has made a payment
        if (row[7] != nullptr && strcmp(row[7], "Paid") == 0) {
            // Display payment details for users who have paid
            cout << setw(15) << "\n\tTotal Amount: " << row[4] << endl;
            cout << setw(15) << "\tPayment Method: " << row[5] << endl;
            cout << setw(15) << "\tBank Name: " << row[6] << endl;
            cout << setw(15) << "\tPayment Status: Paid" << endl;
            cout << setw(15) << "\tPayment Date: " << row[8] << endl;
        }
        else {
            // Display payment status for users who have not paid
            cout << setw(15) << "\tPayment Status: Not Paid. \n\n\t\t-----Please pay within the time priode given!-----" << endl;
        }
    }
    // Free the result set
    mysql_free_result(res);

    cout << "\n\nPress Enter To Back...";
    _getch();
    StudentMenu();
}

void Payment()
{
    system("cls"); // Clear the console screen
    int method, option, SecurityCode;
    string StudentName, BankName, PaymentMethod, TotalAmount, PaymentDate, Amount, cardNum, validity;

    cout << "----------Payment-----------" << endl;
    // Check if the payment entry already exists for the given StudentID
    string check_query = "SELECT PaymentStatus FROM payment WHERE StudentID = '" + GlobalStudentID + "'";
    const char* check_q = check_query.c_str();
    qstate = mysql_query(conn, check_q);
    res = mysql_store_result(conn);

    cout << "\t[1]-Credit Card" << endl; 
    cout << "\t[2]- FPX Method" << endl;
    cout << "\nEnter Payment Method Number: ";
    cin >> method;

    // Assuming there is only one row for the given StudentID
    row = mysql_fetch_row(res);

    if (row != NULL)
    {
        string paymentStatus = row[0];
        if (paymentStatus == "Paid")
        {
            cout << "\nYou have already paid. No further payment is required." << endl;
            cout << "Press Enter To Back...";
            _getch();
            StudentMenu();
        }
    }
    if (method == 1)
    {
        system("cls");
        cout << "----------Credit Card------------" << endl;
        cout << "\n\t  Your Total Amount \n\t\tRM540" << endl;
        cout << "\nEnter Bank Name: ";
        cin.ignore(); // to consume the newline character left in the buffer
        getline(cin, BankName);
        cout << "Enter Card Number: ";
        cin >> cardNum;
        cout << "Validity of Card (MM-YY): ";
        cin >> validity;
        cout << "Enter the payment amount (RM..): ";
        cin >> Amount;
        cout << "Enter Today Date (YYYY-MM-DD): ";
        cin >> PaymentDate;

        // Check if the payment entry already exists for the given StudentID
        string check_query = "SELECT count(*) FROM payment WHERE StudentID = '" + GlobalStudentID + "'";
        const char* check_q = check_query.c_str();
        qstate = mysql_query(conn, check_q);
        res = mysql_store_result(conn);
        row = mysql_fetch_row(res);
        int entryCount = stoi(row[0]);

        if (entryCount > 0)
        {
            // Update existing data
            string update_query = "UPDATE payment SET PaymentMethod = 'Credit Card', BankName = '" + BankName + "', PaymentStatus = 'Paid', PaymentDate = '" + PaymentDate + "' where StudentID = '" + GlobalStudentID + "'";
            const char* q = update_query.c_str();
            qstate = mysql_query(conn, q);

            cout << "\nYour payment for " << " has been processed." << endl;
        }
        else
        {
            // Insert new data
            string insert_query = "INSERT INTO payment (PaymentMethod, BankName, PaymentStatus, PaymentDate, StudentID) VALUES ('Credit Card', '" + BankName + "', 'Paid', '" + PaymentDate + "', '" + GlobalStudentID + "')";
            const char* q = insert_query.c_str();
            qstate = mysql_query(conn, q);

            cout << "\nYour payment for " << " has been processed." << endl;
        }
    }
    else if (method == 2)
    {
        system("cls");
        cout << "----------FPX Method-----------" << endl;
        cout << "\n\tYour Total Amount \n\t\tRM540  " << endl;
        cout << "\nEnter Bank Name: ";
        cin.ignore(); // to consume the newline character left in the buffer
        getline(cin, BankName);
        cout << "Enter the payment amount: ";
        cin >> Amount;
        cout << "Enter payment date (yyyy-mm-dd): ";
        cin >> PaymentDate;
 
        // Check if the payment entry already exists for the given StudentID
        string check_query = "SELECT count(*) FROM payment WHERE StudentID = '" + GlobalStudentID + "'";
        const char* check_q = check_query.c_str();
        qstate = mysql_query(conn, check_q);
        res = mysql_store_result(conn);
        row = mysql_fetch_row(res);
        int entryCount = stoi(row[0]);

        if (entryCount > 0)
        {

            // Update existing data
            string update_query = "UPDATE payment SET PaymentMethod = 'FPX', BankName = '" + BankName + "', PaymentStatus = 'Paid', PaymentDate = '" + PaymentDate + "' where StudentID = '" + GlobalStudentID + "'";
            const char* q = update_query.c_str();
            qstate = mysql_query(conn, q);

            cout << "\nYour payment for " << " has been processed." << endl;
        }
        else
        {
            // Insert new data
            string insert_query = "INSERT INTO payment (PaymentMethod, BankName, PaymentStatus, PaymentDate, StudentID) VALUES ('FPX', '" + BankName + "', 'Paid', '" + PaymentDate + "', '" + GlobalStudentID + "')";
            const char* q = insert_query.c_str();
            qstate = mysql_query(conn, q);

            cout << "\nYour payment for " << " has been processed." << endl;
        }
    }
    else
    {
        cout << "\nInvalid payment method!" << endl;
        _getch();
        StudentMenu();
    }
    cout << "\nPress enter to back...";
    _getch();
     StudentMenu();
}


void HostelReportMenu()
{
    system("cls"); // Clear the console screen
    int choose;
    cout << "-----------Hostel Report----------" << endl;
    cout << "\n\t[1]- View My Reported Issues " << endl;
    cout << "\t[2]- Submit a Report " << endl;
    cout << "\t[3]- Go back" << endl;
    cout << "\nChoose menu: ";
    cin >> choose;

    switch (choose)
    {
    case 1:
        ViewHostelReport();
        break;
    case 2:
        StudentHostelReport();
        break;
    case 3:
        StudentMenu();
        break;
    default:
        cout << "Please choose the option between 1 - 3. Press Enter To Continue...";
        _getch();
        StudentMenu();
    }
}

void StudentHostelReport()
{
    system("cls"); // Clear the console screen
    string RoomID, ReportTitle, ReportDate, Description, ReportStatus, StudentID;
    char choose;

    cout << "---------Hostel Report---------" << endl;
    cin.ignore(1, '\n');
    cout << "Enter your room number (SQ-Q-5-01-C(1)): ";
    getline(cin, RoomID);
    cout << "Enter report title: ";
    getline(cin, ReportTitle);
    cout << "Enter report date (yyyy-mm-dd): ";
    getline(cin, ReportDate);
    cout << "Enter the report about : ";
    getline(cin, Description);

    // Insert the hostel report data into the database
    string insert_query = "INSERT INTO hostel_report (StudentID, RoomID, ReportTitle, ReportDate, Description, ReportStatus, AdminID) VALUES ('" + GlobalStudentID + "', '" + RoomID + "', '" + ReportTitle + "', '" + ReportDate + "', '" + Description + "', 'In Progress', 'A001')";
    const char* q = insert_query.c_str();
    qstate = mysql_query(conn, q);

    if (!qstate)
    {
        cout << endl << "You report have been sent. Press Enter to Continue..." << endl;
        _getch();
        HostelReportMenu();
    }
    else
    {
        cout << "Query Execution Problem!" << mysql_errno(conn) << endl;
    }
    // Allow the user to make another report
    do
    {
        cout << "Do you want make another report? (y/n): ";
        cin >> choose;
        if (choose == 'y' || choose == 'Y')
        {
            StudentHostelReport();
        }
        else if (choose == 'n' || choose == 'N')
        {
            HostelReportMenu();
        }
    } while (choose == 'y' || choose == 'Y' || choose == 'n' || choose == 'N');
}

void ViewHostelReport()
{
    // Student views their report
    system("cls");
    cout << "\t---------Hostel Report---------" << endl;

    // SQL query to retrieve hostel reports for the current student
    string checkUser_query = "SELECT StudentID, RoomID, ReportTitle, ReportDate, Description, ReportStatus, AdminID FROM hostel_report WHERE StudentID = '" + GlobalStudentID + "'";
    const char* cu = checkUser_query.c_str();
    qstate = mysql_query(conn, cu);

    if (!qstate)
    {
        res = mysql_store_result(conn);

        if (mysql_num_rows(res) > 0) // Check if there are any reports
        {
            while (row = mysql_fetch_row(res))
            {
                cout << "\n\tStudentID: " << row[0] << endl
                    << "\tRoom Number: " << row[1] << endl
                    << "\tReport title: " << row[2] << endl
                    << "\tReport Date: " << row[3] << endl
                    << "\tDescription: " << row[4] << endl;

                // Check if ReportStatus is NULL
                if (row[5] == nullptr || strcmp(row[5], "NULL") == 0)
                {
                    cout << "\tReport Status: NULL" << endl;
                }
                else
                {
                    cout << "\tReport Status: " << row[5] << endl;
                }

                // Check if AdminID is NULL
                if (row[6] == nullptr || strcmp(row[6], "NULL") == 0)
                {
                    cout << "\tAdminID: NULL" << endl;
                }
                else
                {
                    cout << "\tAdminID: " << row[6] << endl;
                }
            }
        }
        else
        {
            cout << "No reports found for the student." << endl;
        }
    }
    else
    {
        cout << "\nQuery Execution Problem!" << mysql_errno(conn) << endl;
    }

    cout << "\nPress Enter To Back...";
    _getch();
    HostelReportMenu();
}


void StudentProfile()
{
    system("cls");
    int choose;

    cout << "--------My Profile--------" << endl;
    cout << "\n\t[1]- My Profile" << endl;
    cout << "\t[2]- Change Password" << endl;
    cout << "\t[3]- Go back" << endl;

    cout << "\nChoose menu: ";
    cin >> choose;

    switch (choose)
    {
    case 1:
        ProfileForStudent();
        break;
    case 2:
        ChangeStudentPassword();
        break;
    case 3:
        StudentMenu();
        break;
    default:
        cout << "Please choose the option between 1 - 5. Press Enter To Continue...";
        _getch();
        StudentProfile();
    }
}

void ProfileForStudent() {
    system("cls"); // an alternative method to clear the screen

    cout << "----------My Profile-----------" << endl;
    // SQL query to retrieve the profile information for the current student
    string query = "SELECT StudentID, StudentName, StudentTelNum, Faculty, ICNumber, Province, StudentStatus, Description from student WHERE StudentID = '" +GlobalStudentID+ "'";
    qstate = mysql_query(conn, query.c_str());

    if (qstate != 0)
    {
        cerr << "Query execution error: " << mysql_error(conn) << endl;
        return;
    }

    res = mysql_store_result(conn);
    if (res == nullptr)
    {
        cerr << "Error fetching result: " << mysql_error(conn) << endl;
        return;
    }

    // Fetch and print the results
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        cout << "\nStudentID: " << row[0] << endl;
        cout << "Name: " << row[1] << endl;
        cout << "Tel. Number: " << row[2] << endl;
        cout << "Faculty: " << row[3] << endl;
        cout << "IC Number: " << row[4] << endl;
        cout << "Province: " << row[5] << endl;
        cout << "Status: " << (row[6] ? "Approve" : "Not Approve") << endl;
        cout << "Description: " << row[7] << endl;
    }
    // Free the result set
    mysql_free_result(res);

    cout << "\nPress Enter To Back...";
    _getch();
    StudentProfile();
}

void ChangeStudentPassword()
{
    system("cls");
    // Function to change the user's password
    string newPassword, currentPassword;

    cout << "--------Change Password---------" << endl;
    cout << "\nEnter your current password: ";
    getline(cin, currentPassword);

    char ch;
    while ((ch = _getch()) != 13)
    {
        currentPassword += ch;
        cout << '*';
    }

    // Authenticate the user
    string checkUser_query = "SELECT Password FROM student WHERE StudentID = '" + GlobalStudentID + "'";
    const char* cu = checkUser_query.c_str();
    qstate = mysql_query(conn, cu);

    if (!qstate)
    {
        res = mysql_store_result(conn);
        if (res->row_count == 1)
        {
            row = mysql_fetch_row(res);
            string storedPassword = row[0];

            if (currentPassword == storedPassword){
                cout << "\n\nAuthentication successful. \nEnter your new password: ";
                getline(cin, newPassword);
                string update_query = "UPDATE student SET Password = '" + newPassword + "' where StudentID = '" + GlobalStudentID + "'";
                const char* q = update_query.c_str();
                qstate = mysql_query(conn, q);

                cout << "\nnYour password changed successfully." << endl;
            }
            else{
                cout << "\nInvalid password. Password change failed... Press enter to go back" << endl;
                _getch();
                StudentProfile();
            }
        }
        else{
            cout << "\nInvalid username or password. Authentication failed." << endl;
        }
    }
    else
        cout << "Query Execution Problem!" << mysql_errno(conn) << endl;
    StudentProfile();
}


void AdminLogin()
{
    system("cls");

    string AdminId;
    string Password;

    cout << "--- Login Page ---" << endl;
    cout << "Admin ID : ";
    cin >> AdminId;
    cout << "Password : ";
    char ch;
    while ((ch = _getch()) != 13)
    {
        Password += ch;
        cout << '*';
    }

    // Authenticate the user
    string checkUser_query = "SELECT AdminID FROM admin WHERE AdminID = '" + AdminId + "' and Password = '" + Password + "'";
    const char* cu = checkUser_query.c_str();
    qstate = mysql_query(conn, cu);

    if (!qstate)
    {
        res = mysql_store_result(conn);
        if (res->row_count == 1)
        {
            while (row = mysql_fetch_row(res))
                GlobalAdminID = row[0];
            AdminMenu();
            return;
        }
        else
        {
            char c;
            cout << "\nInvalid username or password. Want to try again? (y/n): ";
            cin >> c;
            if (c == 'y' || c == 'Y')
                AdminLogin();
            else
                main();
        }
    }
    else
        cout << "Query Execution Problem!" << mysql_errno(conn) << endl;
}


void AdminMenu()
{
    int adminMenu;

    system("cls");

    cout << "\n----------Admin Menu------------\n" << endl;
    cout << "\t[1]- Profile" << endl;
    cout << "\t[2]- Blocks" << endl;
    cout << "\t[3]- Rooms" << endl;
    cout << "\t[4]- Students" << endl;
    cout << "\t[5]- Hostel Report" << endl;
    cout << "\t[6]- Payment" << endl;
    cout << "\t[7]- logout" << endl;
    cout << "\nChoose menu: ";
    cin >> adminMenu;

    switch (adminMenu)
    {
    case 1:
        AdminProfile();
        break;
    case 2:
        Blocks();
        break;
    case 3:
        RoomPageAdmin();
        break;
    case 4:
        Students();
        break;
    case 5:
        ViewHostelReport_();
        break;
    case 6:
        AdminStudentPayment();
        break;
    case 7:
        main();
        break;
    default:
        cout << "\nPlease choose between 1 - 7. Press Enter To Continue...";
        _getch();
        system("cls");
        AdminMenu();
        break;
    }
}

void RoomPageAdmin()
{
    system("cls");
    int roommenu;

    cout << "--------ROOM---------" << endl;

    cout << "\n\t[1]- View Rooms" << endl;
    cout << "\t[2]- Add Room" << endl;
    cout << "\t[3]- Delete Room" << endl;
    cout << "\t[4]- Go Back" << endl;
    cout << "\nChoose menu: ";

    cin >> roommenu;

    switch (roommenu)
    {
    case 1:
        ViewRooms();
        break;
    case 2:
        AddRoom();
        break;
    case 3:
        DeleteRoom();
        break;
    case 4:
        AdminMenu();
        break;
    default:
        cout << "Please choose between 1 - 4. Press Enter To Continue...";
        _getch();
        system("cls");
        RoomPageAdmin();
        break;
    }
};

void ViewRooms()
{
    system("cls");
    cout << "\t\t---------List of All Rooms---------" << endl;

    // Separate the query based on RoomStatus
    string queryAvailable = "SELECT RoomID, RoomLevel, BlockID FROM room WHERE RoomStatus = 'Available'";
    string queryNotAvailable = "SELECT RoomID, RoomLevel, BlockID FROM room WHERE RoomStatus = 'Not Available'";

    cout << "\nAvailable Rooms:" << endl;
    qstate = mysql_query(conn, queryAvailable.c_str());

    if (!qstate)
    {
        cout << "Room Number" << "\t\tRoom Level" << "\t\tBlock ID" << endl;

        res = mysql_store_result(conn);
        while (row = mysql_fetch_row(res))
        {
            cout << row[0] << "\t\t" << row[1] << "\t\t" << row[2] << endl;
        }
    }
    else
    {
        cout << "\n\nQuery Execution Problem for Available Rooms!" << mysql_errno(conn) << endl;
    }

    cout << "\n\nNot Available Rooms:" << endl;
    qstate = mysql_query(conn, queryNotAvailable.c_str());

    if (!qstate)
    {
        cout << "Room Number" << "\t\tRoom Level" << "\t\tBlock ID" << endl;

        res = mysql_store_result(conn);
        while (row = mysql_fetch_row(res))
        {
            cout << row[0] << "\t\t" << row[1] << "\t\t" << row[2] << endl;
        }
    }
    else
    {
        cout << "\n\nQuery Execution Problem for Not Available Rooms!" << mysql_errno(conn) << endl;
    }

    cout << "\n\nPress Enter To Back...";
    _getch();
    RoomPageAdmin();
}


void AddRoom()
{
    system("cls");
    string RoomID, RoomLevel, BlockID;
    char choose;
    cout << "----------Insert New Room-----------" << endl;

    cin.ignore(1, '\n');
    cout << "Enter Room ID (SQ-Q-5-01-A(1)): ";
    getline(cin, RoomID);
    cout << "Enter Room Level: ";
    getline(cin, RoomLevel);
    cout << "Enter Block ID: ";
    getline(cin, BlockID);

    // Add data into the database
    string insert_query = "INSERT INTO room (RoomID, RoomLevel,BlockID) VALUES ('" + RoomID + "', '" + RoomLevel + "', '" + BlockID + "')";
    const char* q = insert_query.c_str();
    qstate = mysql_query(conn, q);

    if (!qstate)
    {
        cout << endl << "New room is successful added in database." << endl;
    }
    else
    {
        cout << "\nQuery Execution Problem!" << mysql_errno(conn) << endl;
    }

    do
    {
        cout << "\nDo you want add another equipment? (y/n): ";
        cin >> choose;
        if (choose == 'y' || choose == 'Y')
        {
            AddRoom();
        }
        else if (choose == 'n' || choose == 'N')
        {
            RoomPageAdmin();
        }
    } while (choose == 'y' || choose == 'Y' || choose == 'n' || choose == 'N');

}

void DeleteRoom()
{
    system("cls");
    char choose;
    string RoomID;

    cout << "\t\t--- Delete Room ---" << endl;

    qstate = mysql_query(conn, "SELECT RoomID, RoomStatus, RoomLevel, BlockID FROM room");

    if (!qstate)
    {
        cout << "\nRoom ID" << "\t\tRoom Status" << "\tRoom Level" << "\tBlock ID" << endl;

        res = mysql_store_result(conn);
        while (row = mysql_fetch_row(res))
        {
            cout << row[0] << "\t\t" << row[1] << "\t" << row[2] << "\t" << endl;
        }

        cout << "Choose ID: ";
        cin >> RoomID;
        cout << "\nAre you sure want to delete? " << endl;
        cin >> choose;
        if (choose == 'y' || choose == 'Y')
        {
            //deleted data from database
            string delete_query = "DELETE FROM room WHERE RoomID = '" + RoomID + "'";
            const char* q = delete_query.c_str();
            qstate = mysql_query(conn, q);



            cout << "\nRoom successfully deleted" << endl;
            cout << "Do you want to delete other room? (y/n): " << endl;
            cin >> choose;

            if (choose == 'y' || choose == 'Y')
            {
                DeleteRoom();
            }
            else
            {
                RoomPageAdmin();
            }
        }
        else if (choose == 'n' || choose == 'N')
        {
            RoomPageAdmin();
        }
    }
    else
    {
        cout << "Query Execution Problem!" << mysql_errno(conn) << endl;
    }
}

void Blocks()
{
    int blockmenu;

    system("cls");
    cout << "----------Block-----------" << endl;

    cout << "[1]- View Blocks" << endl;
    cout << "[2]- Add Block" << endl;
    cout << "[3]- Delete Block" << endl;
    cout << "[4]- Go Back" << endl;
    cout << "Choose menu: ";

    cin >> blockmenu;

    switch (blockmenu)
    {
    case 1:
        ViewBlocks();
        break;
    case 2:
        AddBlock();
        break;
    case 3:
        DeleteBlock();
        break;
    case 4:
        AdminMenu();
        break;
    default:
        cout << "Please choose between 1 - 5. Press Enter To Continue...";
        _getch();
        system("cls");
        Blocks();
        break;
    }
}

void ViewBlocks()
{
    system("cls");

    cout << "\t\t--- List of All Blocks ---" << endl;

    qstate = mysql_query(conn, "SELECT BlockID, BlockName, BlockGender, AdminID FROM block");

    if (!qstate)
    {
        cout << "\n" << setw(4) << "BlockID   " << setw(10) << "BlockName    " << setw(10) << "BlockGender    " << setw(4) << "AdminID    " << endl;
        res = mysql_store_result(conn);
        while (row = mysql_fetch_row(res))
        {
            cout << setw(4) << row[0] << "\t" << setw(10) << row[1] << "\t" << setw(10) << row[2] << "\t" << setw(4) << row[3] << "\t" << endl;
        }
    }
    else
    {
        cout << "\nQuery Execution Problem!" << mysql_errno(conn) << endl;
    }

    cout << "\n\nPress Enter To Back...";
    _getch();
    Blocks();

}

void DeleteBlock()
{
    system("cls");
    char choose;
    string BlockID;

    cout << "\t\t--- Delete Block ---" << endl;

    qstate = mysql_query(conn, "SELECT BlockID, BlockName, BlockGender, AdminID FROM block");

    if (!qstate)
    {
        cout << "\nBlock ID" << "\tBlock Name" << "\tBlock Gender" << "\tAdmin ID" << endl;

        res = mysql_store_result(conn);
        while (row = mysql_fetch_row(res))
        {
            cout << row[0] << "\t\t" << row[1] << "\t\t" << row[2] << "\t\t" << row[3] << endl;
        }

        cout << "\nChoose Block ID: ";
        cin >> BlockID;

        cout << "\nAre you sure want to delete? (y/n) " << endl;
        cin >> choose;
        if (choose == 'y' || choose == 'Y')
        {
            //delete data from database
            string delete_query = "DELETE FROM block WHERE BlockID = '" + BlockID + "'";
            const char* q = delete_query.c_str();
            qstate = mysql_query(conn, q);

            cout << "\nBlock successfuly delete" << endl;
            cout << "Do you want to delete other block? (y/n): " << endl;
            cin >> choose;
        
            if (choose == 'y' || choose == 'Y')
            {
                DeleteBlock();
            }
            else
            {
                Blocks();
            }
        }
        else if (choose == 'n' || choose == 'N')
        {
            Blocks();
        }
    }
    else
    {
        cout << "\nQuery Execution Problem!" << mysql_errno(conn) << endl;
    }
}


void AddBlock()
{
    system("cls");
    string BlockID, BlockName, BlockGender, AdminID;
    char choose;
    cout << "-----Insert New Block-----" << endl;

    cin.ignore(1, '\n');
    cout << "Block ID: ";
    getline(cin, BlockID);
    cout << "Block Name: ";
    getline(cin, BlockName);
    cout << "Block Gender: ";
    getline(cin, BlockGender);

    //insert new block into database
    string insert_query = "INSERT INTO block (BlockID, BlockName, BlockGender, AdminID) VALUES ('" + BlockID + "', '" + BlockName + "', '" + BlockGender + "', '" + GlobalAdminID + "')";
    const char* q = insert_query.c_str();
    qstate = mysql_query(conn, q);

    if (!qstate)
    {
        cout << endl << "New block is successful added in database." << endl;
    }
    else
    {
        cout << "Query Execution Problem!" << mysql_errno(conn) << endl;
    }

    do
    {
        cout << "Do you want add another equipment? (y/n): ";
        cin >> choose;
        if (choose == 'y' || choose == 'Y')
        {
            AddBlock();
        }
        else if (choose == 'n' || choose == 'N')
        {
            Blocks();
        }
    } while (choose == 'y' || choose == 'Y' || choose == 'n' || choose == 'N');
}

void AdminProfile()
{
    system("cls");
    int choose;

    cout << "-----My Profile------" << endl;

    cout << "1. Profile" << endl;
    cout << "2. Change Password" << endl;
    cout << "3. Go back" << endl;

    cout << "Choose menu: ";
    cin >> choose;

    switch (choose)
    {
     case 1:
         AdminViewProfile();
         break;
    case 2:
        ChangeAdminPassword();
        break;
    case 3:
        AdminMenu();
        break;
    default:
        cout << "Please choose the option between 1 - 5. Press Enter To Continue...";
        _getch();
        system("cls");
        AdminProfile();
        break;
    }
}

void AdminViewProfile() {
    system("cls"); // to clear the screen
    string AdminID;

    cout << "-----My Profile------" << endl;
    string query = "SELECT AdminID, AdminName, ContactNum, EmailAddress, AdminGender, Password FROM admin WHERE AdminID = '"+GlobalAdminID+"'";

    qstate = mysql_query(conn, query.c_str());

    if (qstate != 0)
    {
        cerr << "Query execution error: " << mysql_error(conn) << endl;
        return;
    }

    res = mysql_store_result(conn);

    if (res == nullptr)
    {
        cerr << "Error fetching result: " << mysql_error(conn) << endl;
        return;
    }

    // Fetch and print the results
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        cout << "\nAdminID: " << row[0] << endl;
        cout << "Name: " << row[1] << endl;
        cout << "Tel. Number: " << row[2] << endl;
        cout << "Email Address: " << row[3] << endl;
        cout << "Gender: " << row[4] << endl;
    }

    // Free the result set
    mysql_free_result(res);

    cout << "\n\nPress Enter To Back...";
    _getch();
    AdminProfile();
}

void ChangeAdminPassword()
{
    system("cls");
    // Function to change the user's password
    string newPassword, currentPassword;

    cout << "----- Change Password ------" << endl;
    cout << "\nEnter your current password: ";
    getline(cin, currentPassword);

    char ch;
    while ((ch = _getch()) != 13)
    {
        currentPassword += ch;
        cout << '*';
    }

    // Authenticate the user
    string checkUser_query = "SELECT Password FROM admin WHERE AdminID = '" + GlobalAdminID + "'";
    const char* cu = checkUser_query.c_str();
    qstate = mysql_query(conn, cu);

    if (!qstate)
    {
        res = mysql_store_result(conn);
        if (res->row_count == 1)
        {
            row = mysql_fetch_row(res);
            string storedPassword = row[0];

            if (currentPassword == storedPassword)
            {
                cout << "\n\nAuthentication successful. \nEnter your new password: ";
                getline(cin, newPassword);
                string update_query = "UPDATE admin SET Password = '" + newPassword + "' WHERE AdminID = '" + GlobalAdminID + "'";
                const char* q = update_query.c_str();
                qstate = mysql_query(conn, q);

                cout << "\nnYour password changed successfully." << endl;
            }
            else
            {
                cout << "\nInvalid password. Password change failed." << endl;
                _getch();
                AdminProfile();
            }
        }
        else
        {
            cout << "\nInvalid username or password. Authentication failed." << endl;
        }
    }
    else
        cout << "Query Execution Problem!" << mysql_errno(conn) << endl;
    AdminProfile();
}

void Students()
{
    system("cls");
    int choose;

    cout << "-----Students------" << endl;
    cout << "\n[1]- Student List" << endl;
    cout << "[2]- Search Student" << endl;
    cout << "[3]- Add Student" << endl;
    cout << "[4]- Go back" << endl;
    cout << "\nEnter the option: " << endl;
    cin >> choose;

    switch (choose)
    {
    case 1:
        StudentDetails();
        break;
    case 2:
        SearchStudent();
        break;
    case 3:
        AddStudent();
        break;
    case 4:
        AdminMenu();
        break;
    default:
        cout << "Please choose between 1 - 3. Press Enter To Continue...";
        _getch();
        system("cls");
        AdminMenu();
        break;
    }
}

void SearchStudent() {
    system("cls");
    cout << "\n\t\t\t\t-------- Search Student --------\n\n";

    string StudentID;
    cin.ignore(1, '\n');
    cout << "Enter Student ID: ";
    getline(cin, StudentID);

    // Construct the SQL query to fetch details for a specific student
    string query = "SELECT s.StudentId, s.StudentName, s.StudentTelNum, s.Faculty, s.ICNumber, s.Province, s.StudentStatus, s.Description, b.BlockName, rr.RoomID, rr.RegisterDate, p.PaymentMethod, p.BankName, p.PaymentStatus, p.PaymentDate "
        "FROM registration_room rr "
        "JOIN room r ON rr.RoomID = r.RoomID "
        "JOIN block b ON r.BlockID = b.BlockID "
        "JOIN student s ON rr.StudentID = s.StudentID "
        "LEFT JOIN payment p ON rr.StudentID = p.StudentID "
        "WHERE s.StudentID = '" + StudentID + "'";  // Include WHERE clause to filter by StudentID

    // Execute the SQL query
    qstate = mysql_query(conn, query.c_str());

    if (qstate != 0) {
        cerr << "Query execution error: " << mysql_error(conn) << endl;
        return;
    }

    // Store the result of the query
    res = mysql_store_result(conn);

    if (res == nullptr) {
        cerr << "Error fetching result: " << mysql_error(conn) << endl;
        return;
    }

    // Fetch and print the results
    MYSQL_ROW row;

    if ((row = mysql_fetch_row(res)) != nullptr) {
        cout << "\nStudent ID: " << row[0] << "\n"
            << "Name: " << row[1] << "\n"
            << "Tel. Number: " << row[2] << "\n"
            << "Faculty: " << row[3] << "\n"
            << "IC Number: " << row[4] << "\n"
            << "Province: " << row[5] << "\n"
            << "Status: " << row[6] << "\n"
            << "Description: " << row[7] << "\n"
            << "Block Name: " << row[8] << "\n"
            << "Room ID: " << row[9] << "\n"
            << "Register Date: " << row[10] << "\n";

        // Check if the user has made a payment
        if (row[13] != nullptr && strcmp(row[13], "Paid") == 0) {
            // Display payment details for users who have paid
            cout << setw(15) << "Payment Status: Paid" << endl;
            cout << setw(15) << "Payment Date: " << row[14] << endl;
        }
        else {
            // Display payment status for users who have not paid
            cout << setw(15) << "Payment Status: Not Paid." << endl;
        }

        char option, choose;
        cout << "\nDid you want to edit the data? (y/n): " << endl;
        cin >> option;

        if (option == 'y' || option == 'Y') {
            string StudentStatus, Description;
            cin.ignore(1, '\n');
            cout << "Enter the student status (Approve/Not Approve): ";
            getline(cin, StudentStatus);
            cout << "Enter description: ";
            getline(cin, Description);

            string update_query1 = "UPDATE student SET StudentStatus = '" + StudentStatus + "' WHERE StudentID = '" + StudentID + "'";
            const char* q1 = update_query1.c_str();
            qstate = mysql_query(conn, q1);

            string update_query2 = "UPDATE student SET Description = '" + Description + "' WHERE StudentID = '" + StudentID + "'";
            const char* q2 = update_query2.c_str();
            qstate = mysql_query(conn, q2);

            cout << endl << "\nDo you want to edit other data?(y/n): ";
            cin >> option;

            if (option == 'y' || option == 'Y')
                SearchStudent();
            else if (option == 'n' || option == 'N')
                Students();
        }
        else {
            Students();
        }

        cout << "\nDo you want to search another student? (y/n): ";
        cin >> choose;

        if (choose == 'y' || choose == 'Y')
            SearchStudent();
        else if (choose == 'n' || choose == 'N')
            AdminMenu();
    }
    else {
        char c;
        cout << "\nInvalid StudentID. Want to try again? (y/n): ";
        cin >> c;

        if (c == 'y' || c == 'Y')
            SearchStudent();
        else {
            cout << "\nPress Enter To Go Back...";
            _getch();
            Students();
        }
    }
}


void StudentDetails()
{
    system("cls");
    cout << "\n\t\t\t\t-------- Student Details --------\n\n";

    string query = "SELECT s.StudentId, s.StudentName, s.StudentTelNum, s.Faculty, s.ICNumber, s.Province, s.StudentStatus, s.Description, b.BlockName, rr.RoomID, rr.RegisterDate, p.PaymentMethod, p.BankName, p.PaymentStatus, p.PaymentDate "
        "FROM registration_room rr "
        "JOIN room r ON rr.RoomID = r.RoomID "
        "JOIN block b ON r.BlockID = b.BlockID "
        "JOIN student s ON rr.StudentID = s.StudentID "
        "LEFT JOIN payment p ON rr.StudentID = p.StudentID";  // Use LEFT JOIN to handle students without payment records

    //string query = "SELECT s.StudentId, s.StudentName, s.StudentTelNum, s.Faculty, s.ICNumber, s.Province, s.StudentStatus, s.Description, b.BlockName, rr.RoomID, rr.RegisterDate, p.PaymentMethod, p.BankName, p.PaymentStatus, p.PaymentDate "
    //    "FROM student s "
    //    "JOIN registration_room rr ON s.StudentID = rr.StudentID "
    //    "JOIN room r ON r.roomID = rr.roomID "
    //    "JOIN block b ON r.BlockID = b.BlockID "
    //    "LEFT JOIN payment p ON rr.StudentID = p.StudentID";


    // Execute the SQL query
    qstate = mysql_query(conn, query.c_str());

    if (qstate != 0)
    {
        cerr << "Query execution error: " << mysql_error(conn) << endl;
        return;
    }

    // Store the result of the query
    res = mysql_store_result(conn);

    if (res == nullptr)
    {
        cerr << "Error fetching result: " << mysql_error(conn) << endl;
        return;
    }

    // Fetch and print the results
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        cout << "\nStudent ID: " << row[0] << "\n"
            << "Name: " << row[1] << "\n"
            << "Tel. Number: " << row[2] << "\n"
            << "Faculty: " << row[3] << "\n"
            << "IC Number: " << row[4] << "\n"
            << "Province: " << row[5] << "\n"
            << "Status: " << row[6] << "\n"
            << "Description: " << row[7] << "\n"
            << "Block Name: " << row[8] << "\n"
            << "Room ID: " << row[9] << "\n"
            << "Register Date: " << row[10] << "\n";

        // Check if the user has made a payment
        if (row[13] != nullptr && strcmp(row[13], "Paid") == 0) {
            // Display payment details for users who have paid
            cout << setw(15) << "Payment Status: Paid" << endl;
            cout << setw(15) << "Payment Date: " << row[14] << endl;
        }
        else {
            // Display payment status for users who have not paid
            cout << setw(15) << "Payment Status: Not Paid." << endl;
        }
    }

    cout << "\nPress Enter To Go Back...";
    _getch();
    Students();
}


void AddStudent()
{
    system("cls");
    string StudentID, StudentName, StudentTelNum, Faculty, ICNumber, Province, StudentStatus, Description, Password, AdminID;
    char choose;

    cout << "----- Add New Student -----" << endl;

    cin.ignore(1, '\n');
    cout << "Student Matric Number: ";
    getline(cin, StudentID);
    cout << "Student Name: ";
    getline(cin, StudentName);
    cout << "Student Tel. Number (without '-'): ";
    getline(cin, StudentTelNum);
    cout << "Student IC Number (without '-'): ";
    getline(cin, ICNumber);
    cout << "Faculty: "; 
    getline(cin, Faculty);
    cout << "Province: ";
    getline(cin, Province);
    cout << "Student Status (Approve/Not Approve): ";
    getline(cin, StudentStatus);
    cout << "Description: ";
    getline(cin, Description);
    cout << "Create Password (if not requested, put the IC number): ";
    getline(cin, Password);

    //insert new data into database
    string insert_query = "INSERT INTO student (StudentID, StudentName, StudentTelNum, Faculty, ICNumber, Province, StudentStatus, Description, Password, AdminID) VALUES ('" + StudentID + "', '" + StudentName + "', '" + StudentTelNum + "', '" + Faculty + "', '" + ICNumber + "', '" + Province + "', '" + StudentStatus + "', '" + Description + "', '" + Password + "', '" + GlobalAdminID + "')";
    const char* q = insert_query.c_str();
    qstate = mysql_query(conn, q);

    if (!qstate)
    {
        cout << endl << "New student is successfully added to the database." << endl;
    }
    else
    {
        cout << "Query Execution Problem!" << mysql_errno(conn) << endl;
    }

    cout << "Do you want to add another student? (y/n): ";
    cin >> choose;

    if (choose == 'y' || choose == 'Y')
    {
        AddStudent();
    }
    else if (choose == 'n' || choose == 'N')
    {
        Students();
    }
}

void ViewHostelReport_()
{
    // Admin to manage the hostel report
    system("cls");
    string AdminID, ReportStatus, StudentID;
    char option;
    cout << "\t\t\t\t--- List of All Hostel Report ---" << endl;

    qstate = mysql_query(conn, "SELECT StudentID, RoomID, ReportTitle, ReportDate, Description, ReportStatus, AdminID FROM hostel_report");

    if (!qstate)
    {
        res = mysql_store_result(conn);
        while (row = mysql_fetch_row(res))
        {
            cout << "\n\tStudentID: " << row[0] << endl
                << "\tRoom Number: " << row[1] << endl
                << "\tReport title: " << row[2] << endl
                << "\tReport Date: " << row[3] << endl
                << "\tDescription: " << row[4] << endl
                << "\tReport Status: " << row[5] << endl
                << "\tAdminID: " << row[6] << endl;
        }
    }
    else
    {
        cout << "Query Execution Problem!" << mysql_errno(conn) << endl;
    }

    cout << "\nDo you want to edit the data? (y/n): ";
    cin >> option;
    if (option == 'y' || option == 'Y')
    {
        cin.ignore(1, '\n');
        cout << "Enter StudentID: ";
        cin >> StudentID;

        cin.ignore(1, '\n');
        cout << "Enter the report status (Done/In Progress): ";
        getline(cin, ReportStatus);

        // Validate ReportStatus
        if (ReportStatus != "Done" && ReportStatus != "In Progress")
        {
            cout << "Error, Invalid Report Status" << endl;
            cout << "Press Enter To Back...";
            _getch();
            AdminMenu();
        }

        string update_query1 = "UPDATE hostel_report SET AdminID = '" + GlobalAdminID + "' WHERE StudentID = '" + StudentID + "'";
        const char* q1 = update_query1.c_str();
        qstate = mysql_query(conn, q1);

        string update_query2 = "UPDATE hostel_report SET ReportStatus = '" + ReportStatus + "' WHERE StudentID = '" + StudentID + "'";
        const char* q2 = update_query2.c_str();
        qstate = mysql_query(conn, q2);

        cout << "\nDo you want to edit other data? (y/n): ";
        cin >> option;

        if (option == 'y' || option == 'Y')
            ViewHostelReport_();
        else if (option == 'n' || option == 'N')
            AdminMenu();
    }
    else
    {
        AdminMenu();
    }

    cout << "\nPress Enter To Back...";
    _getch();
    AdminMenu();
}


void AdminStudentPayment()
{
    system("cls");
    cout << "\n\t\t\t\t-------- Student Payment List --------\n\n";

    // Fetch the payment details
    string query = "SELECT rr.RoomID, p.PaymentID, p.TotalAmount, p.PaymentMethod, p.BankName, p.PaymentStatus, p.PaymentDate, p.StudentID, p.PaymentChecking, p.AdminID "
        "FROM payment p "
        "LEFT JOIN registration_room rr ON p.StudentID = rr.StudentID";

    // Execute the SQL query
    qstate = mysql_query(conn, query.c_str());

    if (qstate != 0)
    {
        cerr << "Query execution error: " << mysql_error(conn) << endl;
        return;
    }

    // Store the result of the query
    res = mysql_store_result(conn);

    if (res == nullptr)
    {
        cerr << "Error fetching result: " << mysql_error(conn) << endl;
        return;
    }

    // Fetch and print the results
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res)) != nullptr)
    {
        cout << "\nRoom Number: " << row[0] << "\n"
            << "Payment ID: " << row[1] << "\n"
            << "Student ID: " << row[7] << "\n"
            << "Payment Checking: " << row[8] << "\n"
            << "Admin ID: " << (row[9] ? row[9] : "NULL") << "\n";

        // Check if the user has made a payment
        if (row[5] != nullptr && strcmp(row[5], "Paid") == 0)
        {
            // Display payment details for users who have paid
            cout << "Payment Status: Paid" << endl;
            cout << "Total Amount: " << row[2] << endl;
            cout << "Payment Method: " << row[3] << endl;
            cout << "Bank Name: " << row[4] << endl;
            cout << "Payment Status: " << row[5] << endl;
            cout << "Payment Date: " << row[6] << endl;
        }
        else
        {
            // Display payment status for users who have not paid
            cout << "Payment Status: Not Paid." << endl;
        }
    }
    char option, choose;
    string StudentID, PaymentChecking;
    cout << "\nDo you want to edit student payment checking? (y/n): ";
    cin >> option;
    if (option == 'y' || option == 'Y')
    {
        cin.ignore(1, '\n');
        cout << "Enter StudentID: ";
        cin >> StudentID;

        cin.ignore(1, '\n');
        cout << "Enter the report status (Done/Not Done): ";
        getline(cin, PaymentChecking);

        // Validate PaymentChecking
        if (PaymentChecking != "Done" && PaymentChecking != "Not Done")
        {
            cout << "Error, Invalid Report Status" << endl;
            cout << "Press Enter To Back...";
            _getch();
            AdminMenu();
        }

        string update_query1 = "UPDATE payment SET AdminID = '" + GlobalAdminID + "' WHERE StudentID = '" + StudentID + "'";
        const char* q1 = update_query1.c_str();
        qstate = mysql_query(conn, q1);

        string update_query2 = "UPDATE payment SET PaymentChecking = '" + PaymentChecking + "' WHERE StudentID = '" + StudentID + "'";
        const char* q2 = update_query2.c_str();
        qstate = mysql_query(conn, q2);

        cout << "\nDo you want to edit other data? (y/n): ";
        cin >> option;

        if (option == 'y' || option == 'Y')
            AdminStudentPayment();
        else if (option == 'n' || option == 'N')
            AdminMenu();
    }
    else
    {
        AdminMenu();
    }
    cout << "\nPress Enter To Go Back...";
    _getch();
    AdminMenu();
}

//void UpdateTotalAmountForStudent(const string& studentID, const string& newTotalAmount) {
//   
//    // Display current total amount
//    string totalAmount = row[1];
//    cout << "\n\tYour Total Amount \n\t\t" << totalAmount << endl;
//
//    // Check if the admin wants to edit the total amount
//    char editOption;
//    cout << "\nDo you want to edit the total amount? (y/n): ";
//    cin >> editOption;
//
//    if (editOption == 'y' || editOption == 'Y') {
//        cout << "Enter new Total Amount: ";
//        string newTotalAmount;
//        cin >> newTotalAmount;
//
//        // Update the total amount in the database for the specified student
//        string update_query = "UPDATE payment SET TotalAmount = '" + newTotalAmount + "' WHERE StudentID = '" + studentID + "'";
//        const char* q = update_query.c_str();
//        qstate = mysql_query(conn, q);
//
//        if (qstate != 0) {
//            cerr << "Query execution error: " << mysql_error(conn) << endl;
//            return;
//        }
//
//        // Display the updated total amount
//        cout << "Updated Total Amount: " << newTotalAmount << endl;
//        cout << "Total Amount updated successfully." << endl;
//    }
//}
//
