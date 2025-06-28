import React, { useState, useEffect, useRef, useCallback } from 'react';
import { initializeApp } from 'firebase/app';
import { getAuth, signInAnonymously, signInWithCustomToken, onAuthStateChanged } from 'firebase/auth';
import { getFirestore, doc, getDoc, addDoc, setDoc, updateDoc, deleteDoc, onSnapshot, collection, query, where, getDocs, limit, writeBatch } from 'firebase/firestore';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer } from 'recharts';

// Raw CSV data for VybeStryke scenarios
const VYBE_STRYKE_SCENARIOS_DATA = `id,scenario,option_A_text,option_A_EI,option_A_RES,option_A_SS,option_B_text,option_B_EI,option_B_RES,option_B_SS,option_C_text,option_C_EI,option_C_RES,option_C_SS,option_D_text,option_D_EI,option_D_RES,option_D_SS
1,"You overhear a group of classmates spreading rumors about a new student. What do you do?","Join in, it's just a joke.",-10,0,-5,"Walk away, it's none of your business.",-5,-5,0,"Quietly tell the new student later that day you heard something and offer support.",15,5,10,"Confront the group directly and tell them to stop.",5,10,-5
2,"You studied hard for a test, but you still failed. How do you react?","Give up on the subject, it's too hard.",-5,-10,0,"Blame the teacher or the test.",-5,-5,0,"Review your mistakes, ask for help, and plan to study differently next time.",10,15,5,"Cry and hide your results from everyone.",-10,-5,-5
3,"You have a great idea for a school project, but you're shy about sharing it with your group.","Keep it to yourself, someone else will think of something.",-5,-5,-5,"Write it down and slip it to the teacher.",-5,-5,-10,"Practice what you'll say and then confidently present your idea.",10,10,10,"Hint at your idea without fully explaining it.",0,0,-5
4,"Your best friend gets an award you secretly wished you had won. How do you feel and what do you do?","Feel jealous and ignore them.",-10,-5,-10,"Offer sincere congratulations and celebrate their success.",15,5,10,"Complain to other friends that it should have been you.",-10,-5,-10,"Pretend you don't care, but feel bitter inside.",-5,-5,-5
5,"You accidentally broke something valuable at a friend's house. What's your immediate response?","Hide it and hope they don't notice.",-5,-5,-5,"Blame it on someone else.",-10,-5,-10,"Apologize immediately and offer to help fix or replace it.",10,10,10,"Run home and avoid your friend.",-10,-10,-15
6,"You're feeling really down and unmotivated. What's a healthy way to lift your spirits?","Stay in bed all day and scroll social media.",-5,-10,-5,"Talk to a trusted adult or friend about how you feel.",15,10,10,"Eat a lot of unhealthy snacks to distract yourself.",-5,-5,-5,"Force yourself to go out and pretend you're fine.",-10,-5,-5
7,"Someone on social media posts something offensive. What's the most responsible action?","Share it with a mocking comment.",-10,-5,-10,"Report the post and block the user.",5,5,10,"Start an online argument with them.",-10,-5,-10,"Ignore it completely.",-5,-5,-5
8,"You have a conflict with a classmate over a group project. How do you handle it?","Do all the work yourself to avoid confrontation.",-5,-5,-5,"Complain about them to other classmates.",-10,-5,-10,"Calmly discuss the issue with them and suggest a compromise.",10,10,15,"Yell at them until they agree with you.",-15,-10,-15
9,"You're trying something new, like learning an instrument, and it's much harder than you expected.","Give up, it's not for you.",-5,-15,-5,"Keep practicing even when it's frustrating, focusing on small improvements.",10,15,5,"Blame the instructor for not being clear enough.",-5,-5,-5,"Only practice when you feel motivated.",-5,-5,-5
10,"A friend asks you to lie to their parents about where you were. What do you do?","Lie for them, they're your friend.",-5,-5,-10,"Refuse to lie and explain why it's not a good idea.",5,10,10,"Tell their parents the truth without telling your friend first.",-10,-5,-15,"Tell your friend you'll think about it, but then avoid them.",-5,-5,-5
11,"You accidentally send a message meant for one friend to a group chat. It's a bit embarrassing.","Immediately delete it and pretend nothing happened.",-5,-5,-5,"Apologize quickly and make a lighthearted joke about it.",10,5,10,"Leave the group chat out of embarrassment.",-10,-5,-10,"Blame your phone for sending it to the wrong chat.",-5,-5,-5
12,"You're feeling overwhelmed by schoolwork and extracurriculars.","Ignore it and hope it goes away.",-5,-10,-5,"Talk to a teacher or counselor about managing your workload.",10,15,10,"Try to do everything, even if you're exhausted.",-5,-5,-5,"Complain constantly to your friends without seeking solutions.",-5,-5,-5
13,"A new trend is popular, and everyone is doing it, but you don't really like it.","Do it anyway to fit in.",-5,-5,-5,"Politely say it's not your thing and stick to what you enjoy.",5,10,10,"Make fun of people who like the trend.",-10,-5,-10,"Avoid everyone until the trend passes.",-5,-5,-5
14,"You witness a friend struggling with a personal problem, but they haven't told anyone.","Tell everyone so they can get help.",-10,-5,-15,"Approach your friend privately and offer a listening ear without judgment.",15,5,10,"Wait for them to come to you.",-5,-5,-5,"Assume someone else will help them.",-5,-5,-5
15,"You receive constructive criticism on a creative project you worked hard on.","Get defensive and argue that they're wrong.",-10,-5,-5,"Listen carefully, ask clarifying questions, and consider their feedback.",15,10,5,"Discard the project completely.",-5,-10,-5,"Pretend to listen but secretly ignore everything.",-5,-5,-5
16,"You accidentally double-book yourself for two important events.","Just pick one and ghost the other.",-5,-5,-10,"Communicate immediately and apologize to both parties, trying to find a solution.",5,10,15,"Hope no one notices.",-5,-5,-5,"Blame the people who invited you for scheduling conflicts.",-5,-5,-5
17,"You're feeling really angry or frustrated about something that happened.","Yell at the next person you see.",-15,-10,-15,"Take a few deep breaths, identify why you're angry, and find a healthy outlet (e.g., exercise, journaling).",20,15,10,"Bottle up your feelings and pretend you're fine.",-10,-5,-5,"Break something to let off steam.",-5,-10,-5
18,"You see someone being left out of a group activity or conversation.","Ignore it, it's not your problem.",-5,-5,-5,"Make an effort to include them and make them feel welcome.",10,5,15,"Whisper about why they're being left out.",-10,-5,-10,"Join the group and pretend you didn't see anything.",-5,-5,-5
19,"You're asked to lead a project or presentation, and you feel nervous.","Decline the offer, saying you're too busy.",-5,-5,-5,"Accept the challenge, prepare thoroughly, and ask for help if needed.",5,15,10,"Accept, but then procrastinate and do a poor job.",-5,-10,-5,"Try to get someone else to do all the work.",-5,-5,-5
20,"You encounter a new idea or perspective that challenges your own beliefs.","Immediately dismiss it as wrong.",-10,-5,-5,"Listen openly, ask questions, and try to understand their viewpoint.",15,10,10,"Start an argument to prove why you are right.",-10,-5,-10,"Avoid discussing it to prevent conflict.",-5,-5,-5
21,"You're feeling overwhelmed by too many responsibilities. What's a good approach?","Do nothing and hope the problems resolve themselves.",-5,-10,-5,"Prioritize tasks, ask for help if needed, and take small steps.",10,15,5,"Try to do everything at once, leading to burnout.",-5,-5,-5,"Complain about how busy you are to everyone.",-5,-5,-5
22,"A friend shares exciting news, but you're having a bad day.","Make it about your bad day, stealing their thunder.",-10,-5,-10,"Put your feelings aside for a moment to genuinely celebrate with them.",15,5,10,"Just give a curt 'that's nice' and change the subject.",-5,-5,-5,"Fake enthusiasm, but feel annoyed inside.",-5,-5,-5
23,"You forgot to do a chore your parents asked you to do. They ask about it.","Lie and say you did it.",-5,-5,-5,"Admit your mistake, apologize, and offer to do it immediately.",10,10,10,"Get angry and say it's not fair.",-10,-5,-5,"Blame your siblings.",-5,-5,-5
24,"You're trying to learn a new skill, but you keep making mistakes.","Get frustrated and quit.",-5,-15,-5,"See each mistake as a learning opportunity and keep practicing.",10,20,5,"Compare yourself negatively to others who are better.",-5,-5,-5,"Ask for help, but then ignore the advice.",-5,-5,-5
25,"A new student joins your class and seems very quiet and alone.","Don't bother them, they probably want to be alone.",-5,-5,-5,"Smile and introduce yourself, inviting them to join your group.",10,5,15,"Stare at them to make them feel more uncomfortable.",-10,-5,-10,"Talk about them with your friends instead of talking to them.",-5,-5,-5
26,"You witness someone being rude to a service worker (e.g., cashier, waiter).","Ignore it, it's not your place to interfere.",-5,-5,-5,"Politely, but firmly, speak up and defend the worker.",5,15,10,"Record it on your phone for social media.",-10,-5,-10,"Join in the rudeness, thinking it's funny.",-10,-5,-10
27,"You receive an unexpected compliment. How do you respond?","Blush and say 'Oh, it's nothing.'",-5,-5,-5,"Say 'Thank You!' sincerely.",10,5,10,"Tell them they're wrong and you're not good at that.",-5,-5,-5,"Try to quickly give them a compliment back.",-5,-5,-5
28,"You're spending too much time on your phone and want to cut back.","Delete all your apps in a fit of rage.",-5,-5,-5,"Set realistic screen time goals and find alternative activities.",10,15,5,"Just keep scrolling, you can't stop anyway.",-5,-10,-5,"Tell everyone you're quitting social media forever, then relapse.",-5,-5,-5
29,"A friend comes to you with a problem they are facing, asking for advice.","Tell them what you would do and insist they follow your advice.",-10,-5,-10,"Listen actively, offer empathy, and help them explore their own solutions.",15,10,15,"Change the subject to your own problems.",-5,-5,-5,"Tell them to just 'get over it'.",-10,-5,-10
30,"You're having a disagreement with a family member. What's the most constructive approach?","Yell until you get your way.",-15,-10,-15,"Listen to their perspective, express your own calmly, and seek common ground.",20,15,15,"Give them the silent treatment for days.",-10,-5,-10,"Run to your room and slam the door.",-5,-5,-5
`;

// Function to parse the CSV data (General purpose for Kwyz data if needed elsewhere)
const parseCSV = (csv) => {
  const lines = csv.trim().split('\n');
  const headers = lines[0].split(',');
  const questions = [];

  for (let i = 1; i < lines.length; i++) {
    const values = lines[i].split(',');
    if (values.length === headers.length) {
      const question = {};
      headers.forEach((header, index) => {
        question[header.trim()] = values[index].trim();
      });
      questions.push(question);
    } else {
      console.warn(`Skipping malformed CSV line: ${lines[i]}`);
    }
  }
  return questions;
};

// Function to parse VybeStryke CSV data
const parseVybeStrykeCSV = (csv) => {
  const lines = csv.trim().split('\n');
  const headers = lines[0].split(',').map(h => h.trim()); // Trim headers too
  const scenarios = [];

  for (let i = 1; i < lines.length; i++) {
    const values = csvToArray(lines[i]); // Use a more robust CSV line parser
    if (values.length === headers.length) {
        const scenario = { options: [] };
        scenario.id = values[headers.indexOf('id')];
        scenario.scenario = values[headers.indexOf('scenario')];

        // Dynamically parse options based on header patterns
        const optionKeys = ['A', 'B', 'C', 'D']; // Assuming up to 4 options
        optionKeys.forEach(key => {
            const textIndex = headers.indexOf(`option_${key}_text`);
            if (textIndex !== -1) {
                const option = { text: values[textIndex], impact: {} };
                const eiIndex = headers.indexOf(`option_${key}_EI`);
                const resIndex = headers.indexOf(`option_${key}_RES`);
                const ssIndex = headers.indexOf(`option_${key}_SS`);

                // Scale impacts by 10 for the 10000 max attribute value (10000 / 1000 = 10)
                // This means original +10 now becomes +100
                if (eiIndex !== -1) option.impact.emotionalIntelligence = parseInt(values[eiIndex] || 0) * 10;
                if (resIndex !== -1) option.impact.resilience = parseInt(values[resIndex] || 0) * 10;
                if (ssIndex !== -1) option.impact.socialSavvy = parseInt(values[ssIndex] || 0) * 10;

                scenario.options.push(option);
            }
        });
        scenarios.push(scenario);
    } else {
        console.warn(`Skipping malformed VybeStryke CSV line (value count mismatch): ${lines[i]}`);
    }
  }
  return scenarios;
};

// Simple CSV line parser that handles commas within quotes
const csvToArray = (text) => {
  let p = '', row = [''], c = 0;
  for (let i = 0; i < text.length; i++) {
    let ch = text[i];
    if (ch === '"') {
      p = p ? '' : '"';
    } else if (ch === ',' && !p) {
      row[++c] = '';
    } else {
      row[c] += ch;
    }
  }
  return row;
};


// Function to shuffle an array (Fisher-Yates algorithm)
const shuffleArray = (array) => {
  let currentIndex = array.length, randomIndex;

  // While there remain elements to shuffle.
  while (currentIndex !== 0) {
    // Pick a remaining element.
    randomIndex = Math.floor(Math.random() * currentIndex);
    currentIndex--;

    // And swap it with the current element.
    [array[currentIndex], array[randomIndex]] = [
      array[randomIndex], array[currentIndex]];
  }
  return array;
};

// Main App Component for Vyral
const App = () => {
  // Flag to track if the component is mounted to prevent state updates on unmounted components
  const isMounted = useRef(false);

  useEffect(() => {
    isMounted.current = true;
    return () => {
      isMounted.current = false;
    };
  }, []);

  // Firebase state variables
  const [db, setDb] = useState(null);
  const [auth, setAuth] = useState(null);
  const [userId, setUserId] = useState(null);
  const [firebaseInitialized, setFirebaseInitialized] = useState(false);

  // App state variables
  const [vybeOfTheDay, setVybeOfTheDay] = useState(
    "Loading Vybe of the Day..."
  );
  const [vybeGenerating, setVybeGenerating] = useState(false); // New state for Vybe of the Day generation
  const [activeTab, setActiveTab] = useState('home'); // Only 'home' and 'profile' now
  const [activeTopTab, setActiveTopTab] = useState('');

  // Goal Tracker states
  const [currentGoal, setCurrentGoal] = useState('');
  const [brokenDownSteps, setBrokenDownSteps] = useState([]);
  const [goalBreakdownLoading, setGoalBreakdownLoading] = useState(false);
  const [goals, setGoals] = useState([]); // State to store user's goals from Firestore
  const [goalProgressData, setGoalProgressData] = useState([]); // Data for goal progress chart


  // Profile states
  const [profileNickname, setProfileNickname] = useState('');
  const [profileBio, setProfileBio] = useState('');
  const [profileTheme, setProfileTheme] = useState('purple-blue'); // Default theme
  const [isProfileLoading, setIsProfileLoading] = useState(true);

  // LyfeBoard (Notes & Folders) states
  const [notes, setNotes] = useState([]);
  const [folders, setFolders] = useState([]);
  const [activeFolderId, setActiveFolderId] = useState('all'); // 'all' for all notes
  const [newNoteText, setNewNoteText] = useState('');
  const [newFolderName, setNewFolderName] = useState('');
  const [draggingNoteId, setDraggingNoteId] = useState(null);
  const [offset, setOffset] = useState({ x: 0, y: 0 }); // Offset from mouse pointer to note top-left corner
  const noteRefs = useRef({}); // To store refs to note DOM elements for drag calculations
  const lyfeBoardAreaRef = useRef(null); // Ref for the main LyfeBoard area

  // VybeStryke states
  const [strykeAttributes, setStrykeAttributes] = useState({
    emotionalIntelligence: 5000, // Initial value out of 10000
    resilience: 5000,
    socialSavvy: 5000,
  });
  const [strykeScenarios, setStrykeScenarios] = useState([]);
  const [currentStrykeScenarioIndex, setCurrentStrykeScenarioIndex] = useState(0);
  const [strykeGameStarted, setStrykeGameStarted] = useState(false);
  const [strykeGameCompleted, setStrykeGameCompleted] = useState(false);
  const [strykeLoading, setStrykeLoading] = useState(true);
  const [strykeCharacterReaction, setStrykeCharacterReaction] = useState(null); // 'good', 'neutral', 'bad'

  // VybeTree state for latest attributes and generated trait
  const [vybeTreeAttributes, setVybeTreeAttributes] = useState(strykeAttributes);
  const [vybeTreeLoading, setVybeTreeLoading] = useState(true);
  const [generatedTrait, setGeneratedTrait] = useState("Click 'Generate My Vybe-Trait ✨'");
  const [traitGenerating, setTraitGenerating] = useState(false);

  // Character movement states for VybeStryke
  const [characterX, setCharacterX] = useState(50); // Character X position on canvas
  const [characterY, setCharacterY] = useState(50); // Character Y position on canvas
  const [characterAnimationState, setCharacterAnimationState] = useState('idle'); // 'idle', 'reacting', 'moving'
  const animationFrameId = useRef(null); // For animation loop cleanup
  const characterMoveDirection = useRef(1); // 1 for right, -1 for left
  const jumpOffset = useRef(0); // For jump animation
  const jumpVelocity = useRef(0); // For jump animation


  // Define available themes (example: Tailwind CSS gradient classes)
  const themes = {
    'purple-blue': 'bg-gradient-to-br from-purple-600 to-indigo-800',
    'green-teal': 'bg-gradient-to-br from-green-600 to-teal-800',
    'pink-orange': 'bg-gradient-to-br from-pink-600 to-orange-800',
    'indigo-red': 'bg-gradient-to-br from-indigo-600 to-red-800'
  };

  // Initialize Firebase and authenticate
  useEffect(() => {
    try {
      const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
      const firebaseConfig = typeof __firebase_config !== 'undefined' ? JSON.parse(__firebase_config) : {};

      if (Object.keys(firebaseConfig).length === 0) {
        console.error("Firebase config is not provided. Please ensure __firebase_config is set.");
        setFirebaseInitialized(false);
        return;
      }

      const app = initializeApp(firebaseConfig);
      const firestore = getFirestore(app);
      const firebaseAuth = getAuth(app);

      setDb(firestore);
      setAuth(firebaseAuth);

      const unsubscribe = onAuthStateChanged(firebaseAuth, async (user) => {
        if (user) {
          if (isMounted.current) setUserId(user.uid);
        } else {
          // Attempt signInAnonymously first, as custom token can be flaky in some envs
          try {
            await signInAnonymously(firebaseAuth);
          } catch (anonError) {
            console.warn("Anonymous sign-in failed, attempting custom token:", anonError);
            if (typeof __initial_auth_token !== 'undefined') {
              try {
                await signInWithCustomToken(firebaseAuth, __initial_auth_token);
              } catch (customTokenError) {
                console.error("Custom token sign-in failed:", customTokenError);
              }
            } else {
              console.warn("No __initial_auth_token provided.");
            }
          }
          if (isMounted.current) setUserId(firebaseAuth.currentUser?.uid || crypto.randomUUID());
        }
        if (isMounted.current) setFirebaseInitialized(true);
      });

      return () => unsubscribe(); // Cleanup auth listener

    }
    catch (error) {
      console.error("Error initializing Firebase:", error);
      if (isMounted.current) setFirebaseInitialized(false);
    }
  }, []); // Empty dependency array means this runs once on mount

  // Fetch Vybe of the Day when Firebase is initialized
  useEffect(() => {
    if (firebaseInitialized && db) {
      const fetchVybeOfTheDay = async () => {
        try {
          const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
          const q = query(collection(db, `artifacts/${appId}/public/data/vybeOfTheDay`));
          const querySnapshot = await getDocs(q);

          if (isMounted.current) {
            if (!querySnapshot.empty) {
              const quotes = [];
              querySnapshot.forEach((doc) => {
                quotes.push(doc.data());
              });
              // Pick a random quote
              const randomQuote = quotes[Math.floor(Math.random() * quotes.length)];
              setVybeOfTheDay(`"${randomQuote.text}" - ${randomQuote.author}`);
            } else {
              setVybeOfTheDay("\"The only way to do great work is to love what you do.\" - Steve Jobs (Default)");
              console.warn("No 'Vybe of the Day' found in Firestore. Please add documents to 'artifacts/{appId}/public/data/vybeOfTheDay'.");
            }
          }
        } catch (error) {
          console.error("Error fetching Vybe of the Day:", error);
          if (isMounted.current) setVybeOfTheDay("Failed to load Vybe of the Day.");
        }
      };
      fetchVybeOfTheDay();
    }
  }, [firebaseInitialized, db]); // Rerun when firebaseInitialized or db changes

  // Fetch user goals when firebase is initialized and userId is available
  useEffect(() => {
    if (firebaseInitialized && db && userId) {
      const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
      const userGoalsRef = collection(db, `artifacts/${appId}/users/${userId}/goals`);

      const unsubscribe = onSnapshot(userGoalsRef, (snapshot) => {
        if (isMounted.current) {
          const fetchedGoals = [];
          snapshot.forEach(doc => {
            fetchedGoals.push({ id: doc.id, ...doc.data() });
          });
          setGoals(fetchedGoals);
          // Process goals for charting
          const dailyGoals = fetchedGoals.filter(goal => goal.completed)
            .reduce((acc, goal) => {
              const date = goal.timestamp ? new Date(goal.timestamp.seconds * 1000).toISOString().split('T')[0] : 'Unknown Date';
              acc[date] = (acc[date] || 0) + 1;
              return acc;
            }, {});

          const chartData = Object.keys(dailyGoals).sort().map(date => ({
            date,
            completed: dailyGoals[date]
          }));
          setGoalProgressData(chartData);
        }
      }, (error) => {
        console.error("Error fetching user goals:", error);
      });

      return () => unsubscribe(); // Cleanup the listener
    }
  }, [firebaseInitialized, db, userId]);

  // Fetch user profile when firebase is initialized and userId is available
  useEffect(() => {
    if (firebaseInitialized && db && userId) {
      const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
      const profileDocRef = doc(db, `artifacts/${appId}/users/${userId}/profile/myProfile`);

      const unsubscribe = onSnapshot(profileDocRef, (docSnap) => {
        if (isMounted.current) {
          if (docSnap.exists()) {
            const data = docSnap.data();
            setProfileNickname(data.nickname || '');
            setProfileBio(data.bio || '');
            setProfileTheme(data.profileTheme || 'purple-blue');
          } else {
            // Initialize with default values if no profile exists
            setProfileNickname('');
            setProfileBio('');
            setProfileTheme('purple-blue');
          }
          setIsProfileLoading(false);
        }
      }, (error) => {
        console.error("Error fetching user profile:", error);
        if (isMounted.current) setIsProfileLoading(false);
      });

      return () => unsubscribe();
    }
  }, [firebaseInitialized, db, userId]);


  // Fetch notes from Firestore
  useEffect(() => {
    if (firebaseInitialized && db && userId) {
      const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
      const notesCollectionRef = collection(db, `artifacts/${appId}/users/${userId}/notes`);
      const unsubscribe = onSnapshot(notesCollectionRef, (snapshot) => {
        if (isMounted.current) {
          const fetchedNotes = [];
          snapshot.forEach(doc => {
            fetchedNotes.push({ id: doc.id, ...doc.data() });
          });
          setNotes(fetchedNotes);
        }
      }, (error) => {
        console.error("Error fetching notes:", error);
      });
      return () => unsubscribe();
    }
  }, [firebaseInitialized, db, userId]);

  // Fetch folders from Firestore
  useEffect(() => {
    if (firebaseInitialized && db && userId) {
      const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
      const foldersCollectionRef = collection(db, `artifacts/${appId}/users/${userId}/folders`);
      const unsubscribe = onSnapshot(foldersCollectionRef, (snapshot) => {
        if (isMounted.current) {
          const fetchedFolders = [{ id: 'all', name: 'All Notes' }]; // Default "All Notes" folder
          snapshot.forEach(doc => {
            fetchedFolders.push({ id: doc.id, ...doc.data() });
          });
          setFolders(fetchedFolders);
        }
      }, (error) => {
        console.error("Error fetching folders:", error);
      });
      return () => unsubscribe();
    }
  }, [firebaseInitialized, db, userId]);

  // Parse VybeStryke scenarios from embedded data
  useEffect(() => {
    if (isMounted.current) setStrykeLoading(true);
    try {
      const parsedScenarios = parseVybeStrykeCSV(VYBE_STRYKE_SCENARIOS_DATA);
      if (isMounted.current) setStrykeScenarios(shuffleArray(parsedScenarios));
      console.log("Stryke Scenarios parsed from embedded data:", parsedScenarios);
    } catch (error) {
      console.error("Error parsing VybeStryke scenarios from embedded data:", error);
      if (isMounted.current) setStrykeScenarios([]); // Clear scenarios if parsing fails
    } finally {
      if (isMounted.current) setStrykeLoading(false);
    }
  }, []); // Run only once on component mount to parse embedded scenarios

  // Fetch latest VybeStryke attributes for VybeTree
  useEffect(() => {
    if (firebaseInitialized && db && userId) {
      const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
      const attributesDocRef = doc(db, `artifacts/${appId}/users/${userId}/attributes/latest`);

      const unsubscribe = onSnapshot(attributesDocRef, (docSnap) => {
        if (isMounted.current) {
          if (docSnap.exists()) {
            setVybeTreeAttributes(docSnap.data());
          } else {
            // Set to default if no attributes are found yet (game not played)
            setVybeTreeAttributes({ emotionalIntelligence: 5000, resilience: 5000, socialSavvy: 5000 });
          }
          setVybeTreeLoading(false);
        }
      }, (error) => {
        console.error("Error fetching VybeTree attributes:", error);
        if (isMounted.current) setVybeTreeLoading(false);
      });

      return () => unsubscribe();
    }
  }, [firebaseInitialized, db, userId]);


  // Handle drag start for notes
  const handleMouseDown = (e, noteId) => {
      setDraggingNoteId(noteId);
      const noteElement = noteRefs.current[noteId];
      if (noteElement) {
          // Calculate offset from mouse to the top-left of the note
          setOffset({
              x: e.clientX - noteElement.getBoundingClientRect().left,
              y: e.clientY - noteElement.getBoundingClientRect().top
          });
      }
  };

  // Global mouse move and mouse up listeners for dragging
  useEffect(() => {
      const handleMouseMove = async (e) => {
          if (draggingNoteId !== null) {
              const lyfeBoardRect = lyfeBoardAreaRef.current.getBoundingClientRect();

              // Calculate new position relative to the LyfeBoardArea
              const newX = e.clientX - lyfeBoardRect.left - offset.x;
              const newY = e.clientY - lyfeBoardRect.top - offset.y;

              // Update state for visual dragging
              if (isMounted.current) {
                setNotes(prevNotes =>
                    prevNotes.map(note =>
                        note.id === draggingNoteId
                            ? { ...note, x: newX, y: newY }
                            : note
                    )
                );
              }
          }
      };

      const handleMouseUp = async () => {
          if (draggingNoteId !== null) {
              const draggedNote = notes.find(note => note.id === draggingNoteId);
              if (draggedNote && db && userId) {
                  try {
                      const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
                      const noteRef = doc(db, `artifacts/${appId}/users/${userId}/notes`, draggedNote.id);
                      await updateDoc(noteRef, { x: draggedNote.x, y: draggedNote.y });
                      console.log("Note position updated in Firestore.");
                  } catch (error) {
                      console.error("Error updating note position:", error);
                  }
              }
              if (isMounted.current) {
                setDraggingNoteId(null);
                setOffset({ x: 0, y: 0 });
              }
          }
      };

      if (draggingNoteId !== null) {
          // Attach to document to ensure drag works even if mouse leaves note
          document.addEventListener('mousemove', handleMouseMove);
          document.addEventListener('mouseup', handleMouseUp);
      }

      return () => {
          document.removeEventListener('mousemove', handleMouseMove);
          document.removeEventListener('mouseup', handleMouseUp);
      };
  }, [draggingNoteId, notes, offset, db, userId]); // Dependencies for drag events

  // Add new note to Firestore
  const addNote = async () => {
    if (!newNoteText.trim() || !db || !userId) return;

    const noteColors = ['bg-yellow-200', 'bg-blue-200', 'bg-green-200', 'bg-pink-200', 'bg-purple-200'];
    const randomColor = noteColors[Math.floor(Math.random() * noteColors.length)];

    try {
        const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
        const notesCollectionRef = collection(db, `artifacts/${appId}/users/${userId}/notes`);
        await addDoc(notesCollectionRef, {
            text: newNoteText,
            x: 50 + Math.random() * 50, // Slight random offset for new notes
            y: 50 + Math.random() * 50,
            color: randomColor,
            folderId: activeFolderId !== 'all' ? activeFolderId : null, // Store null if 'all' is selected
            createdAt: new Date()
        });
        if (isMounted.current) setNewNoteText('');
    } catch (error) {
        console.error("Error adding note:", error);
    }
  };

  // Delete note from Firestore
  const deleteNote = async (noteId) => {
    if (!db || !userId) return;
    try {
        const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
        const noteRef = doc(db, `artifacts/${appId}/users/${userId}/notes`, noteId);
        await deleteDoc(noteRef);
        console.log("Note deleted successfully!");
    } catch (error) {
        console.error("Error deleting note:", error);
    }
  };

  // Add new folder to Firestore
  const addFolder = async () => {
    if (!newFolderName.trim() || !db || !userId) return;
    try {
        const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
        const foldersCollectionRef = collection(db, `artifacts/${appId}/users/${userId}/folders`);
        await addDoc(foldersCollectionRef, {
            name: newFolderName.trim(),
            createdAt: new Date()
        });
        if (isMounted.current) setNewFolderName('');
    } catch (error) {
        console.error("Error adding folder:", error);
    }
  };

  // Delete folder from Firestore and unassign notes
  const deleteFolder = async (folderId) => {
    if (!db || !userId || folderId === 'all') return; // Prevent deleting 'All Notes'

    if (!window.confirm("Are you sure you want to delete this folder? All notes in this folder will become 'unassigned'.")) {
        return; // User cancelled
    }

    try {
        const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
        const folderRef = doc(db, `artifacts/${appId}/users/${userId}/folders`, folderId);
        await deleteDoc(folderRef);

        // Update notes that were in this folder to have no folderId
        const notesToUpdate = notes.filter(note => note.folderId === folderId);
        const batch = writeBatch(db); // Use batch for multiple updates
        notesToUpdate.forEach(note => {
            const noteRef = doc(db, `artifacts/${appId}/users/${userId}/notes`, note.id);
            batch.update(noteRef, { folderId: null });
        });
        await batch.commit();

        if (isMounted.current && activeFolderId === folderId) {
            setActiveFolderId('all'); // Switch to 'All Notes' if current folder is deleted
        }
        console.log("Folder and associated notes updated successfully!");
    } catch (error) {
        console.error("Error deleting folder:", error);
    }
  };

  // Gemini API call for Vybe of the Day generation
  const generateNewVybe = async () => {
    if (isMounted.current) setVybeGenerating(true);
    if (isMounted.current) setVybeOfTheDay("Generating a new Vybe... ✨");
    try {
      const prompt = "Generate a short, inspirational, and teen-friendly motivational quote or lyric, suitable for a 'Vybe of the Day' feature. Provide only the quote/lyric and the author if applicable.";
      let chatHistory = [];
      chatHistory.push({ role: "user", parts: [{ text: prompt }] });
      const payload = { contents: chatHistory };
      const apiKey = ""; // If you want to use models other than gemini-2.0-flash or imagen-3.0-generate-002, provide an API key here. Otherwise, leave this is.
      const apiUrl = `https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?key=${apiKey}`;
      const response = await fetch(apiUrl, {
                 method: 'POST',
                 headers: { 'Content-Type': 'application/json' },
                 body: JSON.stringify(payload)
             });
      const result = await response.json();
      if (isMounted.current) {
        if (result.candidates && result.candidates.length > 0 &&
            result.candidates[0].content && result.candidates[0].content.parts &&
            result.candidates[0].content.parts.length > 0) {
          const text = result.candidates[0].content.parts[0].text;
          setVybeOfTheDay(text);
        } else {
          setVybeOfTheDay("Couldn't generate a new Vybe. Try again!");
          console.error("Gemini API response structure unexpected:", result);
        }
      }
    } catch (error) {
      console.error("Error calling Gemini API for Vybe generation:", error);
      if (isMounted.current) setVybeOfTheDay("Error generating Vybe. Please check console.");
    } finally {
      if (isMounted.current) setVybeGenerating(false);
    }
  };

  // Gemini API call for Goal Breakdown
  const handleBreakDownGoal = async (goalText) => {
    if (isMounted.current) setGoalBreakdownLoading(true);
    if (isMounted.current) setBrokenDownSteps([]); // Clear previous steps
    try {
      const prompt = `Break down the following goal into 3-5 small, actionable, and teen-friendly steps. List them clearly, perhaps with bullet points or numbered items. The goal is: "${goalText}"`;
      let chatHistory = [];
      chatHistory.push({ role: "user", parts: [{ text: prompt }] });
      const payload = { contents: chatHistory };
      const apiKey = "";
      const apiUrl = `https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?key=${apiKey}`;
      const response = await fetch(apiUrl, {
                 method: 'POST',
                 headers: { 'Content-Type': 'application/json' },
                 body: JSON.stringify(payload)
             });
      const result = await response.json();
      if (isMounted.current) {
        if (result.candidates && result.candidates.length > 0 &&
            result.candidates[0].content && result.candidates[0].content.parts &&
            result.candidates[0].content.parts.length > 0) {
          const text = result.candidates[0].content.parts[0].text;
          // Simple parsing for bullet points/numbered lists for display
          const steps = text.split('\n').filter(line => line.trim().length > 0);
          setBrokenDownSteps(steps);
        } else {
          setBrokenDownSteps(["Failed to break down goal. Try rephrasing?"]);
          console.error("Gemini API response structure unexpected:", result);
        }
      }
    } catch (error) {
      console.error("Error calling Gemini API for goal breakdown:", error);
      if (isMounted.current) setBrokenDownSteps(["Error breaking down goal. Please check console."]);
    } finally {
      if (isMounted.current) setGoalBreakdownLoading(false);
    }
  };

  // Save profile changes to Firestore
  const saveProfileChanges = async () => {
    if (!db || !userId) return;
    try {
      const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
      const profileDocRef = doc(db, `artifacts/${appId}/users/${userId}/profile/myProfile`);
      await setDoc(profileDocRef, {
        nickname: profileNickname,
        bio: profileBio,
        profileTheme: profileTheme,
        updatedAt: new Date()
      }, { merge: true }); // Use merge to avoid overwriting other fields
      console.log("Profile saved successfully!");
    } catch (error) {
      console.error("Error saving profile:", error);
    }
  };

  // --- VybeStryke Game Logic ---
  const initializeStrykeGame = () => {
    // Ensure scenarios are loaded before starting
    if (strykeScenarios.length === 0) {
      console.warn("VybeStryke scenarios not loaded yet. Please wait or check embedded data.");
      return;
    }
    setStrykeScenarios(shuffleArray([...strykeScenarios])); // Re-shuffle on game start/restart
    setCurrentStrykeScenarioIndex(0);
    setStrykeAttributes({
      emotionalIntelligence: 5000, // Initial value out of 10000
      resilience: 5000,
      socialSavvy: 5000,
    });
    setStrykeGameStarted(true);
    setStrykeGameCompleted(false);
    setStrykeCharacterReaction(null); // Reset character reaction
    setCharacterAnimationState('moving'); // Start character moving
  };

  const handleStrykeChoice = async (option) => { // Made async to save attributes
    let totalImpact = 0;
    const updatedAttributes = {}; // Use a temp object to update attributes based on current state

    setStrykeAttributes(prevAttributes => {
      const newAttributes = { ...prevAttributes };
      for (const attr in option.impact) {
        const impactValue = option.impact[attr];
        newAttributes[attr] = Math.min(10000, Math.max(0, newAttributes[attr] + impactValue));
        totalImpact += impactValue;
      }
      Object.assign(updatedAttributes, newAttributes); // Capture the new attributes for Firestore save
      return newAttributes;
    });

    // Determine character reaction based on total impact
    if (totalImpact >= 100) { // Threshold for a "good" response (e.g., net +10 from original scale * 10)
      setStrykeCharacterReaction('good');
      setCharacterAnimationState('happy');
    } else if (totalImpact <= -100) { // Threshold for a "bad" response (e.g., net -10 from original scale * 10)
      setStrykeCharacterReaction('bad');
      setCharacterAnimationState('sad');
    } else {
      setStrykeCharacterReaction('neutral');
      setCharacterAnimationState('idle'); // Or 'thoughtful'
    }

    // Save current attributes to Firestore after each choice for persistent VybeTree
    if (db && userId && Object.keys(updatedAttributes).length > 0) { // Ensure updatedAttributes is not empty
      try {
        const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
        const attributesDocRef = doc(db, `artifacts/${appId}/users/${userId}/attributes/latest`);
        await setDoc(attributesDocRef, updatedAttributes, { merge: true }); // Merge to update specific fields
        console.log("Stryke attributes saved to Firestore.");
      } catch (error) {
        console.error("Error saving Stryke attributes:", error);
      }
    }


    // Delay moving to next scenario to show reaction
    setTimeout(() => {
      if (isMounted.current) { // Check if component is still mounted
        if (currentStrykeScenarioIndex < strykeScenarios.length - 1) {
          setCurrentStrykeScenarioIndex(prevIndex => prevIndex + 1);
          setStrykeCharacterReaction(null); // Clear reaction for next scenario
          setCharacterAnimationState('moving'); // Character resumes moving
        } else {
          setStrykeGameCompleted(true);
          setStrykeCharacterReaction(null); // Clear reaction when game ends
          setCharacterAnimationState('idle'); // Character stops moving
          // Final save of attributes at game completion (redundant with per-choice save, but good for finality)
        }
      }
    }, 1500); // Show reaction for 1.5 seconds
  };

  // Gemini API call for VybeTree Trait Generation
  const generateVybeTrait = async () => {
    if (!db || !userId) {
      setGeneratedTrait("Please sign in or wait for Firebase to initialize.");
      return;
    }
    setTraitGenerating(true);
    setGeneratedTrait("Generating your unique Vybe-Trait... ✨");

    const { emotionalIntelligence, resilience, socialSavvy } = vybeTreeAttributes;

    const prompt = `Based on these attributes: Emotional Intelligence: ${emotionalIntelligence}/10000, Resilience: ${resilience}/10000, Social Savvy: ${socialSavvy}/10000. Generate a unique, inspiring, and teen-friendly personality trait/title. Be creative and evocative, like 'The Empathetic Visionary' or 'The Steadfast Innovator'. Keep it concise, a few words long, followed by a very short, positive explanation (1-2 sentences). Do not use the exact attribute names in the trait title.`;

    let chatHistory = [];
    chatHistory.push({ role: "user", parts: [{ text: prompt }] });
    const payload = { contents: chatHistory };
    const apiKey = ""; // If you want to use models other than gemini-2.0-flash or imagen-3.0-generate-002, provide an API key here. Otherwise, leave this as-is.
    const apiUrl = `https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?key=${apiKey}`;

    try {
        const response = await fetch(apiUrl, {
                   method: 'POST',
                   headers: { 'Content-Type': 'application/json' },
                   body: JSON.stringify(payload)
               });
        const result = await response.json();
        if (isMounted.current) {
            if (result.candidates && result.candidates.length > 0 &&
                result.candidates[0].content && result.candidates[0].content.parts &&
                result.candidates[0].content.parts.length > 0) {
              const text = result.candidates[0].content.parts[0].text;
              setGeneratedTrait(text);
            } else {
              setGeneratedTrait("Couldn't generate a trait. Try again!");
              console.error("Gemini API response structure unexpected:", result);
            }
        }
    } catch (error) {
        console.error("Error calling Gemini API for trait generation:", error);
        if (isMounted.current) setGeneratedTrait("Error generating trait. Please check console.");
    } finally {
        if (isMounted.current) setTraitGenerating(false);
    }
  };

  // Canvas drawing and animation logic for VybeStryke
  const canvasRef = useRef(null);
  const characterBaseY = useRef(0); // Stores the ground level Y for the character

  // Draw character and handles animations
  const drawCharacter = useCallback((ctx, x, y, animation, frame) => {
    ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height); // Clear only character area for simpler animations

    const headRadius = 15;
    const bodyLength = 30;
    const legLength = 25;
    const armLength = 20;

    // Draw head
    ctx.beginPath();
    ctx.arc(x, y, headRadius, 0, Math.PI * 2, true);
    ctx.fillStyle = '#6366F1'; // Indigo color
    ctx.fill();
    ctx.strokeStyle = '#4F46E5';
    ctx.lineWidth = 2;
    ctx.stroke();

    // Draw body
    ctx.beginPath();
    ctx.moveTo(x, y + headRadius);
    ctx.lineTo(x, y + headRadius + bodyLength);
    ctx.stroke();

    // Draw legs
    ctx.beginPath();
    ctx.moveTo(x, y + headRadius + bodyLength);
    ctx.lineTo(x - 10, y + headRadius + bodyLength + legLength); // Left leg
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(x, y + headRadius + bodyLength);
    ctx.lineTo(x + 10, y + headRadius + bodyLength + legLength); // Right leg
    ctx.stroke();

    // Draw arms (dynamic for animations)
    ctx.beginPath();
    ctx.moveTo(x, y + headRadius + 10);
    if (animation === 'happy') {
      ctx.lineTo(x - armLength, y + headRadius + 10 - 5); // Arms up for happy/jump
    } else if (animation === 'sad') {
      ctx.lineTo(x - armLength, y + headRadius + 10 + 5); // Arms down for sad
    } else {
      ctx.lineTo(x - armLength, y + headRadius + 10); // Normal arms
    }
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(x, y + headRadius + 10);
    if (animation === 'happy') {
      ctx.lineTo(x + armLength, y + headRadius + 10 - 5); // Arms up for happy/jump
    } else if (animation === 'sad') {
      ctx.lineTo(x + armLength, y + headRadius + 10 + 5); // Arms down for sad
    } else {
      ctx.lineTo(x + armLength, y + headRadius + 10); // Normal arms
    }
    ctx.stroke();

    // Simple expressions
    ctx.fillStyle = '#FFFFFF'; // White eyes
    ctx.beginPath();
    ctx.arc(x - 5, y - 5, 2, 0, Math.PI * 2); ctx.fill();
    ctx.beginPath();
    ctx.arc(x + 5, y - 5, 2, 0, Math.PI * 2); ctx.fill();

    ctx.strokeStyle = '#FFFFFF'; // White mouth
    ctx.lineWidth = 1;
    ctx.beginPath();
    if (animation === 'happy' || animation === 'jump') {
        ctx.arc(x, y + 5, 5, 0, Math.PI, false); // Smile
    } else if (animation === 'sad') {
        ctx.arc(x, y + 10, 5, 0, Math.PI, true); // Frown
    } else {
        ctx.moveTo(x - 5, y + 5);
        ctx.lineTo(x + 5, y + 5); // Straight line for neutral/idle
    }
    ctx.stroke();

    // Small detail for movement animation (leg animation)
    if (animation === 'moving') {
        const legSwing = Math.sin(frame * 0.1) * 5; // Swing legs slightly
        ctx.beginPath();
        ctx.moveTo(x, y + headRadius + bodyLength);
        ctx.lineTo(x - 10 + legSwing, y + headRadius + bodyLength + legLength);
        ctx.stroke();
        ctx.beginPath();
        ctx.moveTo(x, y + headRadius + bodyLength);
        ctx.lineTo(x + 10 - legSwing, y + headRadius + bodyLength + legLength);
        ctx.stroke();
    }


  }, []); // No dependencies for drawCharacter itself


  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas || activeTopTab !== 'vybestrike') {
      if (animationFrameId.current) {
        cancelAnimationFrame(animationFrameId.current);
        animationFrameId.current = null;
      }
      return;
    }

    const ctx = canvas.getContext('2d');
    const container = canvas.parentElement;

    // Set canvas dimensions to match its parent container
    const resizeCanvas = () => {
      canvas.width = container.clientWidth;
      canvas.height = container.clientHeight;
      characterBaseY.current = canvas.height * 0.7 - 70; // Set base Y for character
    };

    resizeCanvas(); // Initial resize
    window.addEventListener('resize', resizeCanvas); // Resize on window resize

    const characterWidth = 30; // Approx width of drawn character
    const characterHeight = 70; // Approx height of drawn character
    let lastTime = 0;
    const moveSpeed = 0.05; // Pixels per ms
    let frame = 0; // For simple animation frames

    const jumpPower = 0.3; // Initial upward velocity for jump
    const gravity = 0.0005; // Gravity effect

    const animate = (currentTime) => {
      if (!isMounted.current) {
        if (animationFrameId.current) cancelAnimationFrame(animationFrameId.current);
        return;
      }

      const deltaTime = currentTime - lastTime;
      lastTime = currentTime;
      frame++;

      ctx.clearRect(0, 0, canvas.width, canvas.height); // Clear entire canvas for redraw

      // Draw simple environment
      ctx.fillStyle = '#E0F2F7'; // Light blue sky
      ctx.fillRect(0, 0, canvas.width, canvas.height * 0.7);
      ctx.fillStyle = '#A8DADC'; // Ground color
      ctx.fillRect(0, canvas.height * 0.7, canvas.width, canvas.height * 0.3);


      setCharacterY(prevY => {
        let newY = prevY;
        if (characterAnimationState === 'happy') { // Only 'happy' triggers jump
          jumpVelocity.current -= gravity * deltaTime;
          newY = characterBaseY.current - jumpOffset.current + jumpVelocity.current * deltaTime; // y is top-left corner
          jumpOffset.current += jumpVelocity.current * deltaTime;

          if (jumpOffset.current <= 0) { // Landed
            jumpOffset.current = 0;
            jumpVelocity.current = 0;
            if (strykeCharacterReaction === 'good') { // Only go to idle/moving if reaction is over
                // This state will be reset by handleStrykeChoice after timeout
            } else {
                setCharacterAnimationState('moving'); // Revert to moving if not a reaction-driven jump
            }
          }
        } else {
          newY = characterBaseY.current; // Stick to ground if not jumping
          jumpOffset.current = 0;
          jumpVelocity.current = 0;
        }
        return newY;
      });

      // Autonomous horizontal movement
      if (strykeGameStarted && !strykeGameCompleted && characterAnimationState === 'moving') {
        setCharacterX(prevX => {
          let newX = prevX + characterMoveDirection.current * moveSpeed * deltaTime;
          if (newX + characterWidth / 2 > canvas.width - 20 || newX - characterWidth / 2 < 20) {
            characterMoveDirection.current *= -1; // Reverse direction
          }
          return newX;
        });
      }

      // Draw character based on state
      drawCharacter(ctx, characterX, characterY + (strykeCharacterReaction === 'happy' ? jumpOffset.current : 0), characterAnimationState, frame);

      animationFrameId.current = requestAnimationFrame(animate);
    };

    if (strykeGameStarted && !strykeGameCompleted && !strykeLoading && activeTopTab === 'vybestrike') {
        setCharacterAnimationState('moving'); // Start moving when game starts
        animationFrameId.current = requestAnimationFrame(animate);
    } else if (!strykeGameStarted || strykeGameCompleted) {
        // If game not started or completed, clear canvas and draw static character
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        ctx.fillStyle = '#E0F2F7'; // Light blue sky
        ctx.fillRect(0, 0, canvas.width, canvas.height * 0.7);
        ctx.fillStyle = '#A8DADC'; // Ground color
        ctx.fillRect(0, canvas.height * 0.7, canvas.width, canvas.height * 0.3);
        const staticCharX = canvas.width / 2;
        const staticCharY = canvas.height * 0.7 - 70;
        drawCharacter(ctx, staticCharX, staticCharY, 'idle', 0); // Draw idle character
    }


    return () => {
      window.removeEventListener('resize', resizeCanvas);
      if (animationFrameId.current) {
        cancelAnimationFrame(animationFrameId.current);
      }
    };
  }, [activeTopTab, strykeGameStarted, strykeGameCompleted, strykeLoading, characterX, characterY, characterAnimationState, drawCharacter]);


  // Function to render content based on the active tabs
  const renderContent = () => {
    // If Firebase is not initialized, show loading or an error message
    if (!firebaseInitialized) {
      return <p className="text-center text-gray-600">Initializing app services...</p>;
    }

    // If a top tab is active, render its content first
    if (activeTopTab) {
      switch (activeTopTab) {
        case 'vybechek':
          return (
            <div className="p-4 flex flex-col items-center">
              <h1 className="text-2xl font-bold text-gray-800 mb-6 text-center">VybeChek: Goal Tracker</h1>
              <div className="w-full max-w-md bg-gray-50 p-6 rounded-xl shadow-lg">
                <h2 className="text-xl font-semibold text-gray-800 mb-4">Add New Goal</h2>
                <input
                  type="text"
                  className="w-full p-3 mb-4 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-indigo-500"
                  placeholder="e.g., Get better grades"
                  value={currentGoal}
                  onChange={(e) => setCurrentGoal(e.target.value)}
                />
                <button
                  className="w-full bg-green-500 text-white py-3 rounded-lg font-semibold shadow-md hover:bg-green-600 transition-all duration-200 active:scale-95 mb-4"
                  onClick={async () => {
                    if (currentGoal.trim()) {
                      try {
                        const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
                        await addDoc(collection(db, `artifacts/${appId}/users/${userId}/goals`), {
                          goal: currentGoal.trim(),
                          timestamp: new Date(),
                          completed: false,
                          steps: [] // Initialize with empty steps
                        });
                        setCurrentGoal('');
                        setBrokenDownSteps([]); // Clear breakdown after adding
                      } catch (error) {
                        console.error("Error adding goal:", error);
                      }
                    }
                  }}
                >
                  Add Goal
                </button>

                {currentGoal.trim() && ( // Show breakdown button only if there's a goal typed
                    <button
                        className="w-full bg-purple-500 text-white py-3 rounded-lg font-semibold shadow-md hover:bg-purple-600 transition-all duration-200 active:scale-95"
                        onClick={() => handleBreakDownGoal(currentGoal)}
                        disabled={goalBreakdownLoading}
                    >
                        {goalBreakdownLoading ? 'Breaking Down... ✨' : 'Break Down Goal ✨'}
                    </button>
                )}

                {brokenDownSteps.length > 0 && (
                    <div className="mt-6 p-4 bg-indigo-50 rounded-lg">
                        <h3 className="text-lg font-semibold text-indigo-800 mb-2">Suggested Steps:</h3>
                        <ul className="list-disc list-inside text-gray-700 space-y-1">
                            {brokenDownSteps.map((step, index) => (
                                <li key={index}>{step.replace(/^- /, '')}</li> // Remove leading bullet point if present
                            ))}
                        </ul>
                    </div>
                )}
              </div>

              {/* Display existing goals */}
              <div className="w-full max-w-md mt-6 bg-white p-6 rounded-xl shadow-lg">
                <h2 className="text-xl font-semibold text-gray-800 mb-4">My Goals</h2>
                {goals.length === 0 ? (
                  <p className="text-gray-600 text-center">No goals added yet. Start tracking!</p>
                ) : (
                  <ul className="space-y-3">
                    {goals.map((goal) => (
                      <li key={goal.id} className="flex items-center justify-between bg-gray-100 p-3 rounded-lg shadow-sm">
                        <span className={`flex-1 text-gray-800 ${goal.completed ? 'line-through text-gray-500' : ''}`}>
                          {goal.goal}
                        </span>
                        <div className="flex items-center space-x-2">
                          <input
                            type="checkbox"
                            checked={goal.completed}
                            onChange={async (e) => {
                              const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
                              await updateDoc(doc(db, `artifacts/${appId}/users/${userId}/goals`, goal.id), {
                                completed: e.target.checked
                              });
                            }}
                            className="form-checkbox h-5 w-5 text-indigo-600 rounded"
                          />
                          <button
                            onClick={async () => {
                              const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';
                              await deleteDoc(doc(db, `artifacts/${appId}/users/${userId}/goals`, goal.id));
                            }}
                            className="text-red-500 hover:text-red-700 transition-colors"
                          >
                            <svg xmlns="http://www.w3.org/2000/svg" className="h-5 w-5" viewBox="0 0 20 20" fill="currentColor">
                              <path fillRule="evenodd" d="M9 2a1 1 0 00-.894.553L7.382 4H4a1 1 0 000 2v10a2 2 0 002 2h8a2 2 0 002-2V6a1 1 0 100-2h-3.382l-.724-1.447A1 1 0 0011 2H9zM7 8a1 1 0 012 0v6a1 1 0 11-2 0V8zm6 0a1 1 0 11-2 0v6a1 1 0 112 0V8z" clipRule="evenodd" />
                            </svg>
                          </button>
                        </div>
                      </li>
                    ))}
                  </ul>
                )}
              </div>
              {/* Goal Progress Graph */}
              <div className="w-full max-w-md mt-6 bg-white p-6 rounded-xl shadow-lg">
                <h2 className="text-xl font-semibold text-gray-800 mb-4">Goals Completed Over Time</h2>
                {goals.length === 0 ? (
                  <p className="text-gray-600 text-center">Complete some goals to see your progress here!</p>
                ) : (
                  <ResponsiveContainer width="100%" height={200}>
                    <LineChart data={goals} margin={{ top: 5, right: 20, left: 10, bottom: 5 }}>
                      <CartesianGrid strokeDasharray="3 3" />
                      <XAxis
                        dataKey="timestamp"
                        tickFormatter={(timestamp) => new Date(timestamp.seconds * 1000).toLocaleDateString('en-US', { month: 'short', day: 'numeric' })}
                      />
                      <YAxis allowDecimals={false} />
                      <Tooltip />
                      <Line type="monotone" dataKey="completed" stroke="#8884d8" activeDot={{ r: 8 }} />
                    </LineChart>
                  </ResponsiveContainer>
                )}
              </div>
            </div>
          );
        case 'vybestrike':
          return (
            <div className="p-4 flex flex-col items-center h-full relative">
              <h1 className="text-2xl font-bold text-gray-800 mb-6 text-center">VybeStryke: Mini RPG Game</h1>

              {strykeLoading ? (
                <p className="text-center text-gray-600">Loading scenarios for VybeStryke...</p>
              ) : strykeScenarios.length === 0 ? (
                <div className="text-center text-gray-600">
                  <p className="mb-4">No VybeStryke scenarios found. Please ensure the embedded data is correct.</p>
                  <button
                    className="bg-purple-600 text-white py-3 px-8 rounded-full font-semibold shadow-lg hover:bg-purple-700 transition-all duration-200 active:scale-95"
                    onClick={() => window.location.reload()} // Simple refresh to re-attempt parsing
                  >
                    Try Reloading Scenarios
                  </button>
                </div>
              ) : (
                <>
                  {/* Canvas for the 2D environment and character */}
                  <canvas ref={canvasRef} className="absolute inset-0 w-full h-full bg-blue-100 rounded-xl"></canvas>

                  {!strykeGameStarted ? (
                    <div className="relative z-10 text-center bg-white bg-opacity-90 p-6 rounded-xl shadow-lg">
                      <p className="text-lg text-gray-700 mb-6">Embark on a journey to become a Truly Vyral Teen!</p>
                      <button
                        className="bg-purple-600 text-white py-3 px-8 rounded-full font-semibold shadow-lg hover:bg-purple-700 transition-all duration-200 active:scale-95"
                        onClick={initializeStrykeGame}
                      >
                        Start Your VybeStryke!
                      </button>
                    </div>
                  ) : strykeGameCompleted ? (
                    <div className="relative z-10 text-center bg-white bg-opacity-90 p-6 rounded-xl shadow-lg">
                      <h2 className="text-2xl font-bold text-gray-800 mb-4">VybeStryke Completed!</h2>
                      <p className="text-lg text-gray-700 mb-6">Your final attributes:</p>
                      <div className="w-full max-w-xs mx-auto space-y-4 mb-8">
                        {Object.entries(strykeAttributes).map(([attr, value]) => (
                          <div key={attr} className="mb-2">
                            <p className="text-sm font-semibold text-gray-700 capitalize">{attr.replace(/([A-Z])/g, ' $1').trim()}</p>
                            <div className="w-full bg-gray-200 rounded-full h-2.5">
                              <div className="bg-gradient-to-r from-purple-500 to-indigo-500 h-2.5 rounded-full transition-all duration-500" style={{ width: `${(value / 10000) * 100}%` }}></div>
                            </div>
                          </div>
                        ))}
                      </div>
                      <button
                        className="bg-purple-600 text-white py-3 px-8 rounded-full font-semibold shadow-lg hover:bg-purple-700 transition-all duration-200 active:scale-95"
                        onClick={initializeStrykeGame}
                      >
                        Play Again!
                      </button>
                    </div>
                  ) : (
                    <div className="relative z-10 w-full max-w-md bg-white bg-opacity-90 p-6 rounded-xl shadow-lg flex flex-col flex-1 justify-between">
                      <div>
                        <p className="text-sm text-gray-500 mb-2">Scenario {currentStrykeScenarioIndex + 1} / {strykeScenarios.length}</p>
                        <p className="text-lg font-semibold text-gray-900 mb-6">{strykeScenarios[currentStrykeScenarioIndex]?.scenario}</p>
                        <div className="space-y-3">
                          {strykeScenarios[currentStrykeScenarioIndex]?.options.map((option, index) => (
                            <button
                              key={index}
                              className="w-full py-3 px-4 rounded-lg text-left text-base font-semibold transition-all duration-200 bg-gray-200 text-gray-800 hover:bg-gray-300 active:scale-95"
                              onClick={() => handleStrykeChoice(option)}
                              disabled={strykeCharacterReaction !== null} // Disable choices when showing reaction
                            >
                              {option.text}
                            </button>
                          ))}
                        </div>
                      </div>

                      <div className="mt-8 pt-4 border-t border-gray-200">
                        <h3 className="text-md font-bold text-gray-800 mb-3 text-center">Your Attributes</h3>
                        <div className="space-y-4">
                          {Object.entries(strykeAttributes).map(([attr, value]) => (
                            <div key={attr} className="mb-2">
                              <p className="text-sm font-semibold text-gray-700 capitalize">{attr.replace(/([A-Z])/g, ' $1').trim()}</p>
                              <div className="w-full bg-gray-200 rounded-full h-2.5">
                                <div className="bg-gradient-to-r from-purple-500 to-indigo-500 h-2.5 rounded-full transition-all duration-500" style={{ width: `${(value / 10000) * 100}%` }}></div>
                              </div>
                            </div>
                          ))}
                        </div>
                      </div>
                    </div>
                  )}
                </>
              )}
            </div>
          );
        case 'vybetree': // New VybeTree content
          return (
            <div className="p-4 flex flex-col items-center">
              <h1 className="text-2xl font-bold text-gray-800 mb-6 text-center">VybeTree: Your Growth Path</h1>
              {vybeTreeLoading ? (
                <p className="text-gray-600 text-center">Loading your VybeTree attributes...</p>
              ) : (
                <div className="w-full max-w-md bg-gray-50 p-6 rounded-xl shadow-lg text-center">
                  <div className="text-5xl mb-4">🌳</div> {/* Tree graphic */}
                  <h2 className="text-xl font-bold text-gray-800 mb-4">Your Current Attributes:</h2>
                  <div className="space-y-4 mb-6">
                    {Object.entries(vybeTreeAttributes).map(([attr, value]) => (
                      <div key={attr} className="mb-2">
                        <p className="text-sm font-semibold text-gray-700 capitalize">{attr.replace(/([A-Z])/g, ' $1').trim()}</p>
                        <div className="w-full bg-gray-200 rounded-full h-2.5">
                          <div className="bg-gradient-to-r from-green-500 to-blue-500 h-2.5 rounded-full transition-all duration-500" style={{ width: `${(value / 10000) * 100}%` }}></div>
                        </div>
                      </div>
                    ))}
                  </div>
                  <p className="text-sm text-gray-600 mb-4">Your traits evolve with your VybeStryke choices!</p>
                  <button
                    onClick={generateVybeTrait}
                    disabled={traitGenerating}
                    className="bg-purple-600 text-white py-2 px-6 rounded-full font-semibold shadow-md hover:bg-purple-700 transition-all duration-200 active:scale-95"
                  >
                    {traitGenerating ? 'Generating Trait... ✨' : 'Generate My Vybe-Trait ✨'}
                  </button>
                  {generatedTrait && generatedTrait !== "Click 'Generate My Vybe-Trait ✨'" && (
                      <div className="mt-6 p-4 bg-purple-100 rounded-lg">
                          <h3 className="text-xl font-semibold text-purple-800 mb-2">Your Unique Trait:</h3>
                          <p className="text-lg text-purple-700 italic">{generatedTrait}</p>
                      </div>
                  )}
                </div>
              )}
            </div>
          );
        default:
          return null;
      }
    } else {
      // Otherwise, render content based on the bottom navigation tab
      switch (activeTab) {
        case 'home':
          // Filter notes based on activeFolderId
          const filteredNotes = activeFolderId === 'all'
            ? notes.filter(note => note.folderId === null || note.folderId === undefined) // 'All Notes' shows notes with no folder
            : notes.filter(note => note.folderId === activeFolderId);

          return (
            <div className="flex flex-col h-full">
              <h1 className="text-2xl font-bold text-gray-800 mb-6 text-center">Your LyfeBoard</h1>

              {/* Add New Note Section */}
              <div className="mb-6 p-4 bg-gray-100 rounded-xl shadow-inner">
                <h2 className="text-lg font-semibold text-gray-800 mb-3">Create New Note</h2>
                <div className="flex space-x-2 mb-3">
                  <input
                    type="text"
                    className="flex-1 p-2 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-indigo-500"
                    placeholder="Write a new note..."
                    value={newNoteText}
                    onChange={(e) => setNewNoteText(e.target.value)}
                    onKeyPress={(e) => {
                      if (e.key === 'Enter') addNote();
                    }}
                  />
                  <button
                    className="bg-indigo-600 text-white p-2 rounded-lg font-semibold shadow-md hover:bg-indigo-700 transition-all duration-200 active:scale-95"
                    onClick={addNote}
                  >
                    Add Note
                  </button>
                </div>

                {/* Folder Management */}
                <h2 className="text-lg font-semibold text-gray-800 mb-3 mt-4">Folders</h2>
                <div className="flex items-center space-x-2 mb-3">
                    <input
                        type="text"
                        className="flex-1 p-2 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-purple-500"
                        placeholder="New folder name..."
                        value={newFolderName}
                        onChange={(e) => setNewFolderName(e.target.value)}
                        onKeyPress={(e) => {
                          if (e.key === 'Enter') addFolder();
                        }}
                    />
                    <button
                        className="bg-purple-600 text-white p-2 rounded-lg font-semibold shadow-md hover:bg-purple-700 transition-all duration-200 active:scale-95"
                        onClick={addFolder}
                    >
                        Add Folder
                    </button>
                </div>
                <div className="flex flex-wrap gap-2">
                  <button
                    className={`px-4 py-2 rounded-full text-sm font-medium transition-all duration-200 ${activeFolderId === 'all' ? 'bg-indigo-500 text-white shadow-md' : 'bg-gray-200 text-gray-700 hover:bg-gray-300'}`}
                    onClick={() => setActiveFolderId('all')}
                  >
                    All Notes
                  </button>
                  {folders.map(folder => (
                    <div key={folder.id} className="relative group">
                      <button
                        className={`px-4 py-2 rounded-full text-sm font-medium transition-all duration-200 ${activeFolderId === folder.id ? 'bg-indigo-500 text-white shadow-md' : 'bg-gray-200 text-gray-700 hover:bg-gray-300'}`}
                        onClick={() => setActiveFolderId(folder.id)}
                      >
                        {folder.name}
                      </button>
                      <button
                        onClick={(e) => { e.stopPropagation(); deleteFolder(folder.id); }}
                        className="absolute -top-1 -right-1 bg-red-500 text-white rounded-full w-5 h-5 flex items-center justify-center text-xs opacity-0 group-hover:opacity-100 transition-opacity duration-200"
                        title="Delete Folder"
                      >
                        ✕
                      </button>
                    </div>
                  ))}
                </div>
              </div>

              {/* Sticky Notes Display Area */}
              <div
                ref={lyfeBoardAreaRef}
                className="flex-1 relative border-2 border-dashed border-gray-300 rounded-xl overflow-auto bg-gray-50 p-4"
                style={{ minHeight: '300px' }} // Ensure visibility even with no notes
              >
                {filteredNotes.length === 0 ? (
                  <p className="absolute inset-0 flex items-center justify-center text-gray-500 italic">
                    {activeFolderId === 'all' ? 'No notes yet! Add one above.' : 'No notes in this folder.'}
                  </p>
                ) : (
                  filteredNotes.map(note => (
                    <div
                      key={note.id}
                      ref={el => noteRefs.current[note.id] = el}
                      className={`absolute ${note.color} p-3 rounded-lg shadow-md cursor-grab active:cursor-grabbing text-gray-800 text-sm font-medium max-w-[150px] break-words`}
                      style={{ left: note.x, top: note.y, zIndex: draggingNoteId === note.id ? 100 : 1 }}
                      onMouseDown={(e) => handleMouseDown(e, note.id)}
                    >
                      {note.text}
                      <button
                        onClick={() => deleteNote(note.id)}
                        className="absolute -top-2 -right-2 bg-red-500 text-white rounded-full w-5 h-5 flex items-center justify-center text-xs shadow-sm hover:bg-red-600 transition-colors"
                        title="Delete Note"
                      >
                        ✕
                      </button>
                    </div>
                  ))
                )}
              </div>
            </div>
          );
        case 'profile':
          if (isProfileLoading) {
            return <p className="text-center text-gray-600">Loading profile...</p>;
          }
          return (
            <div className="p-4 flex flex-col items-center">
              <h1 className="text-2xl font-bold text-gray-800 mb-6 text-center">Your Profile</h1>
              <div className="w-full max-w-md bg-gray-50 p-6 rounded-xl shadow-lg">
                <div className="mb-4 text-center">
                  {/* Profile picture using the selected theme */}
                  <div className={`mx-auto w-24 h-24 rounded-full flex items-center justify-center text-white text-5xl font-bold mb-4 ${themes[profileTheme]}`}>
                    {profileNickname ? profileNickname[0].toUpperCase() : '👤'}
                  </div>
                  <h2 className="text-xl font-semibold text-gray-800 mb-2">{profileNickname || 'Guest User'}</h2>
                  <p className="text-gray-600 text-sm">{profileBio || 'Tell us about yourself!'}</p>
                </div>

                <div className="mb-4">
                  <label htmlFor="nickname" className="block text-sm font-medium text-gray-700 mb-1">Nickname</label>
                  <input
                    type="text"
                    id="nickname"
                    className="w-full p-3 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-indigo-500"
                    value={profileNickname}
                    onChange={(e) => setProfileNickname(e.target.value)}
                    onBlur={saveProfileChanges} // Save on blur
                    placeholder="Your nickname"
                  />
                </div>

                <div className="mb-4">
                  <label className="block text-sm font-medium text-gray-700 mb-2">Profile Theme</label>
                  <div className="flex justify-around space-x-2">
                    {Object.keys(themes).map((themeKey) => (
                      <button
                        key={themeKey}
                        className={`w-10 h-10 rounded-full cursor-pointer border-2 ${themeKey === profileTheme ? 'border-indigo-600 ring-2 ring-indigo-300' : 'border-gray-300'} transition-all duration-200`}
                        onClick={() => {
                            setProfileTheme(themeKey);
                            saveProfileChanges(); // Save immediately when theme changes
                        }}
                        title={themeKey.replace('-', ' ')}
                      ><div className={`w-full h-full rounded-full ${themes[themeKey]}`}></div></button>
                    ))}
                  </div>
                </div>
              </div>
              {/* Current VybeStryke Attributes for Profile */}
              <div className="w-full max-w-md mt-6 bg-white p-6 rounded-xl shadow-lg">
                <h2 className="text-xl font-semibold text-gray-800 mb-4">Your Current Vybe Attributes</h2>
                {vybeTreeLoading ? (
                  <p className="text-gray-600 text-center">Loading attributes...</p>
                ) : (
                  <div className="space-y-4">
                    {Object.entries(vybeTreeAttributes).map(([attr, value]) => (
                      <div key={attr} className="mb-2">
                        <p className="text-sm font-semibold text-gray-700 capitalize">{attr.replace(/([A-Z])/g, ' $1').trim()}</p>
                        <div className="w-full bg-gray-200 rounded-full h-2.5">
                          <div className="bg-gradient-to-r from-purple-500 to-indigo-500 h-2.5 rounded-full transition-all duration-500" style={{ width: `${(value / 10000) * 100}%` }}></div>
                        </div>
                      </div>
                    ))}
                  </div>
                )}
                <p className="text-sm text-gray-600 mt-4">These reflect your choices in VybeStryke.</p>
              </div>
            </div>
          );
        default:
          return null;
      }
    }
  };

  // Handler for bottom navigation clicks
  const handleBottomTabClick = (tabName) => {
    setActiveTab(tabName);
    setActiveTopTab(''); // Deactivate top tab when a bottom tab is clicked
  };

  // Handler for top navigation clicks
  const handleTopTabClick = (tabName) => {
    setActiveTopTab(tabName);
    setActiveTab(''); // Deactivate bottom tab when a top tab is clicked
  };

  return (
    // Main container for the entire app, simulating a mobile device screen
    <div className={`min-h-screen ${themes[profileTheme]} flex items-center justify-center p-4 font-inter`}>
      {/* Mobile-like app container */}
      <div className="relative w-full max-w-md bg-white rounded-3xl shadow-xl overflow-hidden md:max-w-lg lg:max-w-xl h-[85vh] flex flex-col">

        {/* Vybe of the Day Banner */}
        <div className="bg-gradient-to-r from-pink-500 to-orange-400 p-4 text-white text-center rounded-b-xl shadow-md z-10">
          <h2 className="text-sm font-semibold mb-1 uppercase tracking-wider opacity-90">Vybe of the Day</h2>
          <p className="text-lg italic font-medium">
            {vybeOfTheDay}
          </p>
          <button
            onClick={generateNewVybe}
            disabled={vybeGenerating}
            className="mt-2 bg-white bg-opacity-20 hover:bg-opacity-30 text-white text-xs font-semibold py-1 px-3 rounded-full transition-all duration-200 active:scale-95"
          >
            {vybeGenerating ? 'Generating... ✨' : 'Generate New Vybe ✨'}
          </button>
        </div>

        {/* Top Navigation Tabs (VybeChek, VybeStryke, VybeTree) */}
        <div className="bg-gray-100 p-3 flex justify-around border-b border-gray-200 shadow-sm">
          <button
            className={`flex-1 text-sm font-medium py-2 rounded-lg transition-all duration-200 hover:bg-gray-200 active:scale-95
              ${activeTopTab === 'vybechek' ? 'bg-indigo-100 text-indigo-700 font-bold' : 'text-gray-700'}`}
            onClick={() => handleTopTabClick('vybechek')}
          >
            VybeChek
          </button>
          <button
            className={`flex-1 text-sm font-medium py-2 rounded-lg transition-all duration-200 hover:bg-gray-200 active:scale-95
              ${activeTopTab === 'vybestrike' ? 'bg-indigo-100 text-indigo-700 font-bold' : 'text-gray-700'}`}
            onClick={() => handleTopTabClick('vybestrike')}
          >
            VybeStryke
          </button>
          <button
            className={`flex-1 text-sm font-medium py-2 rounded-lg transition-all duration-200 hover:bg-gray-200 active:scale-95
              ${activeTopTab === 'vybetree' ? 'bg-indigo-100 text-indigo-700 font-bold' : 'text-gray-700'}`}
            onClick={() => handleTopTabClick('vybetree')}
          >
            VybeTree
          </button>
        </div>

        {/* Content Area */}
        <div className="flex-1 p-6 overflow-auto relative flex flex-col">
          {renderContent()}
          {/* This is where draggable sticky notes will eventually go (when activeTab is 'home') */}
          <div className="absolute inset-0 pointer-events-none">
            {/* Future area for dynamically placed sticky notes */}
          </div>
        </div>

        {/* User ID Display and Bottom Navigation Tabs (Home, Profile) */}
        <div className="bg-gray-100 p-4 border-t border-gray-200 flex flex-col items-center rounded-t-xl shadow-inner">
          {userId && (
            <p className="text-xs text-gray-500 mb-2">User ID: {userId}</p>
          )}
          <div className="flex justify-around w-full">
            <button
              className={`flex flex-col items-center text-sm font-medium transition-all duration-200 hover:text-indigo-600 active:scale-95
                ${activeTab === 'home' && !activeTopTab ? 'text-indigo-600 font-bold' : 'text-gray-700'}`}
              onClick={() => handleBottomTabClick('home')}
            >
              <svg xmlns="http://www.w3.org/2000/svg" className="h-6 w-6 mb-1" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M3 12l2-2m0 0l7-7 7 7M5 10v10a1 1 0 001 1h3m10-11l2 2m-2-2v10a1 1 0 01-1 1h-3m-6 0a1 1 0 001 1h3v-9a1 1 0 00-1-1H9a1 1 0 00-1 1v9m-6 0h6" />
              </svg>
              Home
            </button>
            <button
              className={`flex flex-col items-center text-sm font-medium transition-all duration-200 hover:text-indigo-600 active:scale-95
                ${activeTab === 'profile' && !activeTopTab ? 'text-indigo-600 font-bold' : 'text-gray-700'}`}
              onClick={() => handleBottomTabClick('profile')}
            >
              <svg xmlns="http://www.w3.org/2000/svg" className="h-6 w-6 mb-1" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M16 7a4 4 0 11-8 0 4 4 0 018 0zM12 14a7 7 0 00-7 7h14a7 7 0 00-7-7z" />
              </svg>
              Profile
            </button>
          </div>
        </div>
      </div>
    </div>
  );
};

export default App;
