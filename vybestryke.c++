#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <thread>
#include <chrono>
// --- Player Definition (VybeStrike) ---
struct Player {
    std::string name;
    int level;
    int xp; // Experience points
    int vybePoints; // Points representing emotional wellness
    int vybuX; // Virtual currency for in-game purchases
    std::map<std::string, int> skills; // Emotional skills and their levels
    // Constructor to initialize player with default values
    Player(std::string playerName) : name(playerName), level(1), xp(0), vybePoints(100), vybuX(50) {
        // Initialize emotional skills with default values
        skills["Empathy"] = 0;
        skills["Self-Awareness"] = 0;   
        skills["Self-Esteem"] = 0;
        skills["Emotional Regulation"] = 0;
        skills["Social Skills"] = 0;
        skills["Problem Solving"] = 0;
        skills["Decision Making"] = 0;
        skills["Stress Management"] = 0;
        skills["Communication"] = 0;
        skills["Conflict Resolution"] = 0;
        skills["Active Listening"] = 0;
        skills["Assertiveness"] = 0;
        skills["Adaptability"] = 0;
        skills["Mindfulness"] = 0;
        skills["Gratitude"] = 0;
        skills["Positive Thinking"] = 0;
        skills["Self-Control"] = 0;
        skills["Creativity"] = 0;
        skills["Resilience"] = 0;
    }


    // Method to display player's current stats
    void displayStats() const {
        std::cout << "\n--- " << name << "'s Stats ---" << std::endl;
        std::cout << "Level: " << level << " (XP: " << xp << "/100)" << std::endl; // Assuming 100 XP per level
        std::cout << "Vybe Points: " << vybePoints << std::endl;
        std::cout << "VybuX: " << vybuX << std::endl;
        std::cout << "Emotional Skills:" << std::endl;
        for (const auto& pair : skills) {
            std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        }
        std::cout << "----------------------" << std::endl;
    }


    // Method to gain XP and potentially level up
    void gainXP(int amount) {
        xp += amount;
        std::cout << name << " gained " << amount << " XP!" << std::endl;
        while (xp >= 100) { // Handle multiple level-ups
            levelUp();
            xp -= 100; // Carry over excess XP
        }
    }


    // Method to handle leveling up
    void levelUp() {
        level++;
        vybePoints += 10; // Increase Vybe Points on level up
        std::cout << "🎉 " << name << " leveled up to Level " << level << "! 🎉" << std::endl;
    }


    // Method to update a specific emotional skill
    void updateSkill(const std::string& skillName, int amount) {
        if (skills.count(skillName)) { // Check if skill exists
            skills[skillName] += amount;
            if (amount > 0) {
                std::cout << name << " " << skillName << " skill increased by " << amount << "!" << std::endl;
            } else if (amount < 0) {
                std::cout << name << " " << skillName << " skill decreased by " << -amount << "!" << std::endl;
            } else {
                std::cout << name << " " << skillName << " skill unchanged." << std::endl;
            }
        } else {
            std::cout << "Error: Skill '" << skillName << "' does not exist." << std::endl;
        }
    }
};


// --- Quiz System Definition (VybeKwyz) ---


// Struct to hold a single quiz question
struct QuizQuestion {
    std::string prompt;
    std::vector<std::string> options;
    int correctAnswerIndex; // 0-indexed
    std::string explanation; // To provide learning moments
    int vybuXReward;
    int xpReward;
    std::string skillToInfluence; // Which skill this quiz influences
    int skillInfluenceAmount;


    // Constructor for easy initialization
    QuizQuestion(std::string p, std::vector<std::string> opts, int ansIdx, std::string exp,
                 int vybuX, int xp, std::string skill, int skillAmt) :
        prompt(p), options(opts), correctAnswerIndex(ansIdx), explanation(exp),
        vybuXReward(vybuX), xpReward(xp), skillToInfluence(skill), skillInfluenceAmount(skillAmt) {}
};


// Function to simulate a single quiz session
void conductQuiz(Player& player, const QuizQuestion& question) {
    std::cout << "\n--- VybeKwyz Time! ---" << std::endl;
    std::cout << "Prompt: " << question.prompt << std::endl;


    for (int i = 0; i < question.options.size(); ++i) {
        std::cout << "  " << (char)('A' + i) << ") " << question.options[i] << std::endl;
    }


    char userAnswerChar;
    int userAnswerIndex = -1;
    bool validInput = false;


    while (!validInput) {
        std::cout << "Your answer (A, B, C, etc.): ";
        std::cin >> userAnswerChar;
        if (std::isalpha(userAnswerChar)) {
            userAnswerChar = toupper(userAnswerChar); // Convert to uppercase for case-insensitivity
        }


        if (userAnswerChar >= 'A' && userAnswerChar < 'A' + question.options.size()) {
            userAnswerIndex = userAnswerChar - 'A';
            validInput = true;
        } else {
            std::cout << "Invalid input. Please choose from A, B, C, etc." << std::endl;
        }
    }


    if (userAnswerIndex == question.correctAnswerIndex) {
        std::cout << "🌟 Correct! 🌟" << std::endl;
        player.vybuX += question.vybuXReward;
        player.gainXP(question.xpReward);
        player.updateSkill(question.skillToInfluence, question.skillInfluenceAmount);
        std::cout << "You gained " << question.vybuXReward << " VybuX and " << question.xpReward << " XP!" << std::endl;
    } else {
        std::cout << "😔 Incorrect. The correct answer was: "
                  << question.options[question.correctAnswerIndex] << std::endl;
        std::cout << "Explanation: " << question.explanation << std::endl;
        player.vybePoints -= 5; // Example: Lose some Vybe Points for incorrect answer
        std::cout << "You lost 5 Vybe Points. Current Vybe Points: " << player.vybePoints << std::endl;
    }
    std::cout << "----------------------" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Pause for readability
}


// --- VybeStrike (Life Scenario Simulation) ---
// Simulates a "life scenario" prompt that affects player stats


struct LifeScenario {
    std::string prompt;
    std::string outcomePositive;
    std::string outcomeNegative;
    int vybePointsChangePositive;
    int vybePointsChangeNegative;
    int xpChangePositive;
    int xpChangeNegative;
    std::string skillToInfluence; // Which skill this scenario influences
    int skillInfluenceAmount;


    LifeScenario(std::string p, std::string op, std::string on, int vpp, int vpn, int xpp, int xpn, std::string skill, int skillAmt) :
        prompt(p), outcomePositive(op), outcomeNegative(on),
        vybePointsChangePositive(vpp), vybePointsChangeNegative(vpn),
        xpChangePositive(xpp), xpChangeNegative(xpn),
        skillToInfluence(skill), skillInfluenceAmount(skillAmt) {}
};


void presentLifeScenario(Player& player, const LifeScenario& scenario) {
    std::cout << "\n--- VybeStrike: Life Scenario! ---" << std::endl;
    // Show the scenario prompt and explain choices
    std::cout << scenario.prompt << std::endl;
    std::cout << "Type 'good' for a positive response, or anything else for a negative response: ";
    std::string choice;
    std::cin >> choice;


    // Convert input to lowercase for case-insensitive comparison
    for (auto& c : choice) c = std::tolower(static_cast<unsigned char>(c));


    // Improved logic: positive outcome increases skill, negative decreases
    if (choice == "good") {
        std::cout << scenario.outcomePositive << std::endl;
        player.vybePoints += scenario.vybePointsChangePositive;
        player.gainXP(scenario.xpChangePositive);
        player.updateSkill(scenario.skillToInfluence, scenario.skillInfluenceAmount); // Increase skill
    } else {
        std::cout << scenario.outcomeNegative << std::endl;
        player.vybePoints += scenario.vybePointsChangeNegative;
        player.gainXP(scenario.xpChangeNegative);
        player.updateSkill(scenario.skillToInfluence, -scenario.skillInfluenceAmount); // Decrease skill
    }
    std::cout << "Current Vybe Points: " << player.vybePoints << std::endl;
    std::cout << "----------------------" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Pause for readability
}


// --- Main function to demonstrate the logic ---
int main() {
    // Seed the random number generator
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());


    // Create a player
    Player currentPlayer("VyralTeen");
    currentPlayer.displayStats();


    // --- Example VybeKwyz Questions ---
    std::vector<QuizQuestion> quizQuestions = {
        QuizQuestion("Your friend is upset because they didn't get invited to a party. What's the best way to respond?",
                     {"Tell them to just get over it.", "Listen to them and validate their feelings.", "Ignore them, it's not your problem."},
                     1, "Active listening and empathy are key to supporting friends.", 10, 15, "Empathy", 1),
        QuizQuestion("You're feeling overwhelmed by homework and extracurriculars. What's a healthy coping mechanism?",
                     {"Procrastinate and hope it goes away.", "Talk to a trusted adult or friend about it.", "Isolate yourself and try to power through."},
                     1, "Reaching out for support is a sign of strength, not weakness.", 15, 20, "Self-Control", 1),
        QuizQuestion("You made a mistake in front of your class. What's the most constructive thought to have?",
                     {"I'm so stupid, everyone is judging me.", "It's okay, everyone makes mistakes, and I can learn from this.", "I'll just pretend it didn't happen."},
                     1, "Self-compassion and a growth mindset help you recover from setbacks.", 12, 18, "Resilience", 1),
        QuizQuestion("You see a classmate struggling with a subject you excel in. What should you do?",
                     {"Ignore them, it's not your problem.", "Offer to help them understand the material.", "Make fun of them for not understanding."},
                     1, "Offering help fosters a supportive learning environment.", 10, 15, "Empathy", 1),
        QuizQuestion("A new student joins your class and seems shy. What could you do?",
                     {"Wait for them to approach you.", "Invite them to join your group at lunch.", "Ignore them, they’ll make friends eventually."},
                     1, "Reaching out helps others feel included and builds empathy.", 10, 15, "Empathy", 1),
        QuizQuestion("You see someone being bullied online. What is a positive action?",
                     {"Join in to fit in.", "Report the behavior and offer support to the victim.", "Ignore it, it's not your business."},
                     1, "Standing up against bullying and supporting others shows courage and empathy.", 15, 20, "Empathy", 1),
        QuizQuestion("You failed a test you studied hard for. What should you do next?",
                     {"Give up on the subject.", "Ask your teacher for feedback and try to improve.", "Blame the teacher for being unfair."},
                     1, "Seeking feedback and trying again builds resilience.", 12, 18, "Resilience", 1),
        QuizQuestion("Your friend wants to copy your homework. What is the best response?",
                     {"Let them copy to keep the peace.", "Explain why it's not okay and offer to help them understand the material.", "Ignore their request."},
                     1, "Helping others learn is better than enabling dishonesty.", 10, 15, "Self-Control", 1),
        QuizQuestion("You have a big presentation and feel nervous. What can help?",
                     {"Practice and use calming techniques.", "Skip the presentation.", "Complain to your friends."},
                     0, "Preparation and self-calming strategies help manage anxiety.", 10, 15, "Resilience", 1),
        QuizQuestion("You notice a friend has been unusually quiet and withdrawn. What do you do?",
                     {"Give them space and say nothing.", "Check in and let them know you care.", "Tell others about their behavior."},
                     1, "Checking in shows empathy and support.", 10, 15, "Empathy", 1)
    };

    // --- Example VybeStrike Scenarios ---
    std::vector<LifeScenario> lifeScenarios = {
        LifeScenario("You forgot your locker combo again. You're running late for class!",
                     "You calmly ask a friend for help, or find a teacher. You learn to write it down.",
                     "You panic, get frustrated, and miss the beginning of class.",
                     5, -10, 10, -5, "Self-Control", 1),
        LifeScenario("A classmate spreads a rumor about you. What's your internal response?",
                     "You understand that people sometimes say mean things when they're struggling, and you decide to ignore it or address it calmly.",
                     "You become very angry and plot revenge, or withdraw completely.",
                     10, -15, 15, -10, "Empathy", 1),
        LifeScenario("You have a creative project due, but you're experiencing writer's block.",
                     "You try brainstorming new ideas, take a short break, or ask for peer feedback.",
                     "You give up and turn in something incomplete or uninspired.",
                     8, -8, 12, -8, "Creativity", 1),
        // Additional life scenarios for teens:
        LifeScenario("You see a group of students excluding someone at lunch.",
                     "You invite the excluded student to join you, making them feel welcome.",
                     "You ignore the situation and sit with your usual friends.",
                     10, -8, 12, -6, "Empathy", 1),
        LifeScenario("You have a disagreement with your parents about curfew.",
                     "You calmly explain your perspective and listen to theirs.",
                     "You yell and slam your door.",
                     7, -10, 10, -7, "Self-Control", 1),
        LifeScenario("You want to try out for a team but are afraid of failing.",
                     "You decide to try out anyway, focusing on the experience.",
                     "You don't try out to avoid disappointment.",
                     9, -7, 11, -6, "Resilience", 1),
        LifeScenario("Your friend is upset after losing a game.",
                     "You encourage them and remind them it's just one game.",
                     "You tease them about losing.",
                     8, -8, 10, -8, "Empathy", 1),
        LifeScenario("You have multiple assignments due and feel overwhelmed.",
                     "You make a plan and tackle one task at a time.",
                     "You procrastinate and avoid your work.",
                     10, -10, 12, -10, "Self-Control", 1)
    };




    // Game loop simulation
    for (int i = 0; i < 3; ++i) { // Simulate 3 rounds of interaction
        std::cout << "\n--- Round " << i + 1 << " ---" << std::endl;


        // Choose a random quiz question
        std::uniform_int_distribution<> quizDist(0, quizQuestions.size() - 1);
        conductQuiz(currentPlayer, quizQuestions[quizDist(rng)]);
        currentPlayer.displayStats();


        // Choose a random life scenario
        std::uniform_int_distribution<> scenarioDist(0, lifeScenarios.size() - 1);
        presentLifeScenario(currentPlayer, lifeScenarios[scenarioDist(rng)]);
        currentPlayer.displayStats();


        // Introduce an emotional skill tree growth visualization concept
        if (i == 1) { // Example trigger for skill tree growth (could be based on skill points)
            std::cout << "\n--- VybeTree Alert! ---" << std::endl;
            std::cout << "Your 'Empathy' branch on the VybeTree is growing stronger!" << std::endl;
            // In a real app, this would be a visual update.
        }
    }


    std::cout << "\n--- Game Over (for this demo)! ---" << std::endl;
    currentPlayer.displayStats();


    return 0;
}
// This code simulates a simple emotional wellness game for teens, focusing on emotional skills,
// quizzes, and life scenarios. It can be expanded with more features, better UI, and
// deeper emotional skill development pathways.