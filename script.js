// Back to Top Button
document.querySelectorAll('#back-to-top').forEach(button => {
    button.addEventListener('click', function(e) {
        e.preventDefault();
        window.scrollTo({ top: 0, behavior: 'smooth' });
    });
});

// Feature Details Toggle
document.querySelectorAll('.feature-toggle').forEach(button => {
    button.addEventListener('click', function() {
        const details = this.nextElementSibling;
        const isVisible = details.style.display === 'block';
        details.style.display = isVisible ? 'none' : 'block';
        this.textContent = isVisible ? 'Show Details' : 'Hide Details';
    });
});

// File Structure Toggle (for details.html)
const fileToggle = document.getElementById('file-toggle');
if (fileToggle) {
    fileToggle.addEventListener('click', function() {
        const details = document.getElementById('file-details');
        const isVisible = details.style.display === 'block';
        details.style.display = isVisible ? 'none' : 'block';
        this.textContent = isVisible ? 'Show Files' : 'Hide Files';
    });
}

// Team Member Thank You Popup (for details.html)
function showThankYou(name) {
    alert(`Thank you, ${name}, for your contribution to the Scalable Thread Management Library!`);
}

// Thread Pool Demo (for index.html)
let demoRunning = false;
let tasks = [];
let threads = [];
const maxThreads = 3;
const taskDurations = [2000, 3000, 1500, 2500, 1000]; // Simulated task durations in ms

function createTask(id) {
    return { id: id, priority: Math.floor(Math.random() * 3), duration: taskDurations[Math.floor(Math.random() * taskDurations.length)] };
}

function updateTaskQueue() {
    const taskList = document.getElementById('tasks');
    if (!taskList) return;
    taskList.innerHTML = '';
    tasks.forEach(task => {
        const li = document.createElement('li');
        li.textContent = `Task ${task.id} (Priority: ${task.priority})`;
        taskList.appendChild(li);
    });
}

function updateThreadPool() {
    const threadDiv = document.getElementById('threads');
    if (!threadDiv) return;
    threadDiv.innerHTML = '';
    threads.forEach((task, index) => {
        const div = document.createElement('div');
        div.textContent = task ? `Thread ${index + 1}: Processing Task ${task.id} (Priority: ${task.priority})` : `Thread ${index + 1}: Idle`;
        threadDiv.appendChild(div);
    });
}

function startDemo() {
    if (demoRunning) return;
    demoRunning = true;
    document.getElementById('start-demo').disabled = true;
    document.getElementById('stop-demo').disabled = false;

    // Initialize threads
    threads = Array(maxThreads).fill(null);

    // Create initial tasks
    tasks = [];
    for (let i = 1; i <= 5; i++) {
        tasks.push(createTask(i));
    }
    tasks.sort((a, b) => b.priority - a.priority); // Sort by priority (descending)
    updateTaskQueue();
    updateThreadPool();

    // Simulate task processing
    const interval = setInterval(() => {
        if (!demoRunning) {
            clearInterval(interval);
            return;
        }

        // Add new tasks periodically
        if (Math.random() < 0.3 && tasks.length < 10) {
            tasks.push(createTask(tasks.length + 1));
            tasks.sort((a, b) => b.priority - a.priority);
            updateTaskQueue();
        }

        // Assign tasks to threads
        for (let i = 0; i < maxThreads; i++) {
            if (!threads[i] && tasks.length > 0) {
                const task = tasks.shift();
                threads[i] = task;
                updateTaskQueue();
                updateThreadPool();

                // Simulate task execution
                setTimeout(() => {
                    if (!demoRunning) return;
                    threads[i] = null;
                    updateThreadPool();
                }, task.duration);
            }
        }
    }, 1000);
}

function stopDemo() {
    demoRunning = false;
    tasks = [];
    threads = Array(maxThreads).fill(null);
    updateTaskQueue();
    updateThreadPool();
    document.getElementById('start-demo').disabled = false;
    document.getElementById('stop-demo').disabled = true;
}

const startDemoButton = document.getElementById('start-demo');
const stopDemoButton = document.getElementById('stop-demo');
if (startDemoButton && stopDemoButton) {
    startDemoButton.addEventListener('click', startDemo);
    stopDemoButton.addEventListener('click', stopDemo);
}