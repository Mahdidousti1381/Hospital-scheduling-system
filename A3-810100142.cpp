
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>

using namespace std;

struct WorkingShift {
    vector<string> day_of_week;
    vector<int> startTime;
    vector<int> endTime;
    vector<int> day_of_week_coded;
    vector<int> startMinute;
};
struct Doctor {
    string name;
    string expertise;
    int visitFee;
    int visitTime;
    int waitTime;
    WorkingShift workTime;
    vector<int> visit_number_in_a_day;
};

struct Patient {
    string name;
    string medical_issue;
    string appointment_day;
    int appointment_day_coded = 0;
    int appointment_hour = 0;
    string relevant_expertise;


};
struct Assigned {
    Patient patient;
    string medical_issue;
    string relevant_expertise;
    vector<Doctor *> related_doctors;
    Doctor *chosen_doctor;
    string visit_day;
    int visit_number = 0;
    int visit_hour = 0;
    int visit_minutes = 0;
    bool ASSIGNED = false;
    int visit_day_coded = 0;
};
struct Expertise {
    string expertiseName;
    vector<string> diseaseName;
};

void workshift_sorter(Doctor &dws) {
    for (int i = 0; i < dws.workTime.day_of_week_coded.size(); i++) {
        for (int j = i + 1; j < dws.workTime.day_of_week_coded.size(); j++) {
            if (dws.workTime.day_of_week_coded[i] > dws.workTime.day_of_week_coded[j]) {
                swap(dws.workTime.day_of_week_coded[i], dws.workTime.day_of_week_coded[j]);
                swap(dws.workTime.startTime[i], dws.workTime.startTime[j]);
                swap(dws.workTime.startMinute[i], dws.workTime.startMinute[j]);
                swap(dws.workTime.endTime[i], dws.workTime.endTime[j]);
                swap(dws.workTime.day_of_week[i], dws.workTime.day_of_week[j]);
            }
        }
    }
}

void seperate_day_hour(string const &day_hour, string &day_of_week, int &hour) {
    string delimiter = "-";
    size_t pos = day_hour.find(delimiter);
    day_of_week = day_hour.substr(0, pos);
    hour = stoi(day_hour.substr(pos + 1));
}

void get_patients_csv(vector<Patient> &patients, string &patients_file) {
    ifstream file(patients_file);
    string line;
    int i = 0;
    getline(file, line);
    while (getline(file, line)) {
        stringstream ss(line);
        string name, medical_issue, appointment_time;
        getline(ss, name, ',');
        getline(ss, medical_issue, ',');
        getline(ss, appointment_time, ',');
        Patient patient = {name, medical_issue};
        patients.push_back(patient);
        seperate_day_hour(appointment_time, patients[i].appointment_day, patients[i].appointment_hour);
        i++;
    }
}

WorkingShift schedule_detect(string &scheduleText) {
    WorkingShift workShift;
    stringstream ss(scheduleText);
    string word;
    while (getline(ss, word, '$')) {
        stringstream ssWord(word);
        string startHour, endHour, dayOfWeek;
        getline(ssWord, dayOfWeek, '-');
        getline(ssWord, startHour, '-');
        getline(ssWord, endHour);
        workShift.day_of_week.push_back(dayOfWeek);
        workShift.startTime.push_back(stoi(startHour));
        workShift.endTime.push_back(stoi(endHour));
        workShift.startMinute.push_back(0);

    }
    return workShift;
}

void init_zero(vector<Doctor> &doctors) {
    for (int j = 0; j < doctors.size(); j++) {
        for (int k = 0; k < doctors[j].workTime.day_of_week.size(); k++) {
            doctors[j].visit_number_in_a_day.push_back(0);
        }
    }
}

int day_of_week_Coder(const string &day) {
    int day_coded = 8;
    if (day == "Sat") {
        day_coded = 0;
    } else if (day == "Sun") {
        day_coded = 1;
    } else if (day == "Mon") {
        day_coded = 2;
    } else if (day == "Tue") {
        day_coded = 3;
    } else if (day == "Wed") {
        day_coded = 4;
    } else if (day == "Thu") {
        day_coded = 5;
    } else if (day == "Fri") {
        day_coded = 6;
    }
    return day_coded;
}

void doctors_working_day_coder(vector<Doctor> &doctors) {
    for (int i = 0; i < doctors.size(); i++) {
        for (int j = 0; j < doctors[i].workTime.day_of_week.size(); j++)
            doctors[i].workTime.day_of_week_coded.push_back(day_of_week_Coder(doctors[i].workTime.day_of_week[j]));
        workshift_sorter(doctors[i]);
    }
}

void get_doctors_csv(vector<Doctor> &doctors, string &doctors_file) {
    ifstream file(doctors_file);
    string line;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string visitFee, visitTime, waitTime;
        string name, expertise, schedule;
        getline(ss, name, ',');
        getline(ss, expertise, ',');
        getline(ss, visitFee, ',');
        getline(ss, visitTime, ',');
        getline(ss, waitTime, ',');
        getline(ss, schedule, ',');
        Doctor doctor = {name, expertise, stoi(visitFee), stoi(visitTime), stoi(waitTime),
                         schedule_detect(schedule)};
        doctors.push_back(doctor);
    }
    init_zero(doctors);
}

vector<string> disease_detect(string &diseasesText) {
    vector<string> illnessNames;
    stringstream ss(diseasesText);
    string word;
    while (getline(ss, word, '$')) {
        stringstream ssWord(word);
        string illnessName;
        getline(ssWord, illnessName);
        illnessNames.push_back(illnessName);
    }
    return illnessNames;
}

void get_disease_csv(vector<Expertise> &expertises, string &diseases_file) {
    ifstream file(diseases_file);
    string line, line1;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string expertise_name, diseasesText;
        getline(ss, expertise_name, ',');
        getline(ss, diseasesText, ',');
        Expertise tempExpertise = {expertise_name, disease_detect(diseasesText)};
        expertises.push_back(tempExpertise);
    }
}

void patients_appointment_day_coder(vector<Patient> &patients) {
    for (int i = 0; i < patients.size(); i++) {
        patients[i].appointment_day_coded = day_of_week_Coder(patients[i].appointment_day);
    }
}

bool compare_patients(const Patient &a, const Patient &b) {
    if (a.appointment_day_coded == b.appointment_day_coded) {
        if (a.appointment_hour == b.appointment_hour)
            return a.name < b.name;
        else
            return a.appointment_hour < b.appointment_hour;
    } else {
        return a.appointment_day_coded < b.appointment_day_coded;
    }
}

void patiensts_sorter(vector<Patient> &patients) {
    patients_appointment_day_coder(patients);
    sort(patients.begin(), patients.end(), compare_patients);
}

void find_relevant_expertise_for_disease(vector<Expertise> &expertises, Patient &patient) {
    for (int i = 0; i < expertises.size(); i++) {
        for (int j = 0; j < expertises[i].diseaseName.size(); j++)
            if (patient.medical_issue == expertises[i].diseaseName[j])
                patient.relevant_expertise = expertises[i].expertiseName;
    }
}

void find_relevant_expertise_for_disease_of_all_patients(vector<Patient> &patients, vector<Expertise> &expertises) {
    for (int i = 0; i < patients.size(); i++)
        find_relevant_expertise_for_disease(expertises, patients[i]);
}

void
find_doctors_with_related_expertise(vector<Patient> &patients, vector<Doctor> &doctors, vector<Assigned> &assignments) {
    for (int i = 0; i < patients.size(); i++) {
        //vector<string> related_doctors;
        for (int j = 0; j < doctors.size(); j++) {
            if (patients[i].relevant_expertise == doctors[j].expertise) {
                assignments[i].related_doctors.push_back(&doctors[j]);
            }
        }
        assignments[i].patient = patients[i];
        assignments[i].relevant_expertise = patients[i].relevant_expertise;
    }
}

void hour_minute_counter(int &minute, int &hour) {
    if (minute >= 60) {
        hour += floor(minute / 60);
        minute = minute % 60;
    }
}

bool compare_doctors(const Doctor *a, const Doctor *b) {
    if (a->workTime.day_of_week_coded[0] == b->workTime.day_of_week_coded[0]) {
        if (a->workTime.startTime[0] == b->workTime.startTime[0]) {
            if (a->workTime.startMinute[0] == b->workTime.startMinute[0]) {
                if (a->visitFee == b->visitFee) {
                    if (a->waitTime == b->waitTime)
                        return a->name < b->name;
                    else
                        return a->waitTime < b->waitTime;
                } else {
                    return a->visitFee < b->visitFee;
                }
            } else {
                return a->workTime.startMinute[0] < b->workTime.startMinute[0];
            }
        } else {
            return a->workTime.startTime[0] < b->workTime.startTime[0];
        }
    } else {
        return a->workTime.day_of_week_coded[0] < b->workTime.day_of_week_coded[0];
    }
}

void erase_doctor_shift(Assigned &assignment) {
    int i = 0;
    int j = 0;
    assignment.related_doctors[i]->workTime.startTime.erase(
            assignment.related_doctors[i]->workTime.startTime.begin());
    assignment.related_doctors[i]->workTime.startMinute.erase(
            assignment.related_doctors[i]->workTime.startMinute.begin());
    assignment.related_doctors[i]->workTime.day_of_week.erase(
            assignment.related_doctors[i]->workTime.day_of_week.begin());
    assignment.related_doctors[i]->workTime.day_of_week_coded.erase(
            assignment.related_doctors[i]->workTime.day_of_week_coded.begin());
    assignment.related_doctors[i]->workTime.endTime.erase(assignment.related_doctors[i]->workTime.endTime.begin());
    assignment.related_doctors[i]->visit_number_in_a_day[j] = 0;
}

bool check_if_doctor_shift_is_over(Assigned &assignment) {
    int i = 0;
    if (assignment.related_doctors[i]->workTime.startTime.empty()) {
        if (assignment.related_doctors.empty()) {
            assignment.ASSIGNED = false;
            return false;
        }
        assignment.related_doctors.erase(assignment.related_doctors.begin());
        assignment.ASSIGNED = false;
        return false;
    }
    return true;
}

void apply_assign(Assigned &assignment) {
    int i = 0;
    int j = 0;

    if (!check_if_doctor_shift_is_over(assignment)) {
        return;
    }
    assignment.visit_hour = assignment.related_doctors[i]->workTime.startTime[j];
    assignment.visit_minutes = assignment.related_doctors[i]->workTime.startMinute[j];
    assignment.related_doctors[i]->workTime.startMinute[j] += assignment.related_doctors[i]->visitTime;
    hour_minute_counter(assignment.related_doctors[i]->workTime.startMinute[j],
                        assignment.related_doctors[i]->workTime.startTime[j]);
    assignment.chosen_doctor = assignment.related_doctors[i];
    assignment.visit_day = assignment.related_doctors[i]->workTime.day_of_week[j];
    assignment.visit_day_coded = assignment.related_doctors[i]->workTime.day_of_week_coded[j];
    assignment.related_doctors[i]->visit_number_in_a_day[j] += 1;
    assignment.visit_number = assignment.related_doctors[i]->visit_number_in_a_day[j];
    assignment.ASSIGNED = true;
    if (assignment.related_doctors[i]->workTime.startTime[j] == assignment.related_doctors[i]->workTime.endTime[j])
        erase_doctor_shift(assignment);
}

bool choose_doctors(Assigned &assignment) {
    sort(assignment.related_doctors.begin(), assignment.related_doctors.end(), compare_doctors);
    while (!assignment.ASSIGNED) {
        for (int k = 0; k < assignment.related_doctors.size() + 100; k++) {
            apply_assign(assignment);
            if (!assignment.ASSIGNED)
                continue;
            return true;
        }
        return false;
    }
}

string week_day_decoder(int const &day) {
    switch (day) {
        case 0 :
            return "Sat";
        case 1:
            return "Sun";
        case 2:
            return "Mon";
        case 3:
            return "Tue";
        case 4:
            return "Wed";
        case 5:
            return "Thu";
        case 6:
            return "Fri";
        default:
            return "0";
    }
}

void full_result_printer(Assigned &assignment) {
    cout << "Name: " << assignment.patient.name << endl;
    cout << "Doctor: " << assignment.chosen_doctor->name << endl;
    if (assignment.visit_hour < 10) {
        if (assignment.visit_minutes < 10)
            cout << "Visit: " << week_day_decoder(assignment.visit_day_coded) << " " << assignment.visit_number << " 0"
                 << assignment.visit_hour << ":0" << assignment.visit_minutes << endl;
        else
            cout << "Visit: " << week_day_decoder(assignment.visit_day_coded) << " " << assignment.visit_number << " 0"
                 << assignment.visit_hour << ":" << assignment.visit_minutes << endl;
    } else if (assignment.visit_hour >= 10) {
        if (assignment.visit_minutes < 10)
            cout << "Visit: " << week_day_decoder(assignment.visit_day_coded) << " " << assignment.visit_number << " "
                 << assignment.visit_hour << ":0" << assignment.visit_minutes << endl;
        else
            cout << "Visit: " << week_day_decoder(assignment.visit_day_coded) << " " << assignment.visit_number << " "
                 << assignment.visit_hour << ":" << assignment.visit_minutes << endl;
    }
    cout << "Charge: " << assignment.chosen_doctor->visitFee << endl;
}

void NoFreeTime_result_printer(Assigned &assignment) {
    cout << "Name: " << assignment.patient.name << endl;
    cout << "No free time" << endl;
}

void print_assigned_assignment(Assigned &assignment, int &count, vector<Assigned> const &assignments) {
    if (count == assignments.size() - 1) {
        full_result_printer(assignment);
    } else {
        full_result_printer(assignment);
        cout << "----------" << endl;
    }
}

void print_not_assigned_assignment(Assigned &assignment, int &count, vector<Assigned> const &assignments) {
    if (count == assignments.size() - 1) {
        NoFreeTime_result_printer(assignment);
    } else {
        NoFreeTime_result_printer(assignment);
        cout << "----------" << endl;
    }
}

void assign_every_patient(vector<Assigned> assignments, vector<Assigned> &final_assignments) {
    for (int i = 0; i < assignments.size(); i++) {
        if (choose_doctors(assignments[i]))
            final_assignments.push_back(assignments[i]);
        else
            final_assignments.push_back(assignments[i]);
    }
}

bool alphabet_sort(Assigned const &a, Assigned const &b) {
    return a.patient.name < b.patient.name;
}

void final_assignment_sort(vector<Assigned> &final_assignments) {
    sort(final_assignments.begin(), final_assignments.end(), alphabet_sort);
    for (int i = 0; i < final_assignments.size(); i++) {
        if (final_assignments[i].ASSIGNED) {
            print_assigned_assignment(final_assignments[i], i, final_assignments);
        } else
            print_not_assigned_assignment(final_assignments[i], i, final_assignments);

    }
}

int main() {
    vector<Patient> patients;
    vector<Doctor> doctors;
    vector<Expertise> expertises;
    vector<Assigned> final_assignments;
    string patients_file = "/Users/Asus/Desktop/University/term 5/AP/CAs/CA3/patients.csv";
    string doctors_file = "/Users/Asus/Desktop/University/term 5/AP/CAs/CA3/doctors.csv";
    string diseases_file = "/Users/Asus/Desktop/University/term 5/AP/CAs/CA3/diseases.csv";
    get_patients_csv(patients, patients_file);
    vector<Assigned> assignments(patients.size());
    get_doctors_csv(doctors, doctors_file);
    get_disease_csv(expertises, diseases_file);
    patiensts_sorter(patients);
    doctors_working_day_coder(doctors);
    find_relevant_expertise_for_disease_of_all_patients(patients, expertises);
    find_doctors_with_related_expertise(patients, doctors, assignments);
    assign_every_patient(assignments, final_assignments);
    final_assignment_sort(final_assignments);

    return 0;

}
