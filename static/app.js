const cpuCtx = document.getElementById('cpuChart').getContext('2d');
const ramCtx = document.getElementById('ramChart').getContext('2d');

const cpuChart = new Chart(cpuCtx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'CPU %',
            data: [],
            borderColor: '#00f0ff',
            backgroundColor: 'rgba(0, 240, 255, 0.1)',
            borderWidth: 2,
            fill: true,
            tension: 0.3,
            pointRadius: 3
        }]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
            y: { beginAtZero: true, max: 100, grid: { color: 'rgba(255, 255, 255, 0.05)' }, ticks: { color: '#8a99ad' } },
            x: { grid: { display: false }, ticks: { color: '#8a99ad', maxTicksLimit: 6 } }
        },
        plugins: { legend: { display: false } }
    }
});

const ramChart = new Chart(ramCtx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'RAM %',
            data: [],
            borderColor: '#b000ff',
            backgroundColor: 'rgba(176, 0, 255, 0.1)',
            borderWidth: 2,
            fill: true,
            tension: 0.3,
            pointRadius: 3
        }]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
            y: { beginAtZero: true, max: 100, grid: { color: 'rgba(255, 255, 255, 0.05)' }, ticks: { color: '#8a99ad' } },
            x: { grid: { display: false }, ticks: { color: '#8a99ad', maxTicksLimit: 6 } }
        },
        plugins: { legend: { display: false } }
    }
});

const evtSource = new EventSource('/stream');

evtSource.onmessage = function(event) {
    const data = JSON.parse(event.data);
    const now = new Date().toLocaleTimeString();

    const cpuValEl = document.getElementById('cpuValue');
    const ramValEl = document.getElementById('ramValue');

    if (cpuValEl) cpuValEl.textContent = `${data.cpu.toFixed(1)}%`;
    if (ramValEl) ramValEl.textContent = `${data.ram.toFixed(1)}%`;

    if (data.disk_total > 0) {
        const diskPercent = (data.disk_used / data.disk_total) * 100;
        document.getElementById('diskValue').textContent = `${data.disk_used.toFixed(1)} GB / ${data.disk_total.toFixed(1)} GB`;
        document.getElementById('diskBar').style.width = `${diskPercent}%`;
    }

    if (data.net_recv > 1024) {
        document.getElementById('netValue').textContent = `${(data.net_recv / 1024).toFixed(2)} MB/s`;
    } else {
        document.getElementById('netValue').textContent = `${data.net_recv.toFixed(2)} KB/s`;
    }

    if (data.procs && data.procs.length > 0) {
        let procHtml = '';
        data.procs.forEach(p => {
            procHtml += `<div class="flex justify-between items-center bg-gray-800/40 p-2 rounded border border-gray-700/40">
                <span class="truncate w-28 text-cyan-300 font-semibold">${p.name}</span>
                <span class="text-gray-500">PID: ${p.pid}</span>
                <span class="text-purple-400 font-bold">${p.mem.toFixed(1)} MB</span>
            </div>`;
        });
        document.getElementById('processList').innerHTML = procHtml;
    }

    if (cpuChart.data.labels.length > 25) {
        cpuChart.data.labels.shift();
        cpuChart.data.datasets[0].data.shift();
        ramChart.data.labels.shift();
        ramChart.data.datasets[0].data.shift();
    }

    cpuChart.data.labels.push(now);
    cpuChart.data.datasets[0].data.push(data.cpu);

    ramChart.data.labels.push(now);
    ramChart.data.datasets[0].data.push(data.ram);

    cpuChart.update();
    ramChart.update();
};