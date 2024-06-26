const deviceToken = "3a0f7dee-f6f3-4765-af08-1b419ef03559"; // Seu token de dispositivo TagoIO
const distanceThreshold = 20; // Defina o valor de distância para considerar a vaga ocupada

// Inicializa o mapa com a localização de Florianópolis
const map = L.map('map').setView([-27.5954, -48.5480], 13);
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    attribution: '&copy; OpenStreetMap contributors'
}).addTo(map);

// Função para criar ou atualizar um marcador
function updateMarker(slotData, index) {
    const distance = slotData.value;
    const markerIconUrl = distance === 0 ? 'red-icon.png' : 'blue-icon.png';
    const markerLocation = [-27.5954 + 0.000 * index, -48.5480 + 0.000 * index];

    let existingMarker = map.markers ? map.markers[index] : null;

    if (existingMarker) {
        // Atualiza o ícone do marcador existente
        existingMarker.setIcon(L.icon({
            iconUrl: markerIconUrl,
            iconSize: [25, 41],
            iconAnchor: [12, 41],
            popupAnchor: [1, -34],
            shadowSize: [41, 41]
        }));
        existingMarker.setPopupContent(distance === 0 ? 'Vaga ocupada' : 'Vaga livre');
    } else {
        // Cria um novo marcador
        const marker = L.marker(markerLocation, {
            icon: L.icon({
                iconUrl: markerIconUrl,
                iconSize: [25, 41],
                iconAnchor: [12, 41],
                popupAnchor: [1, -34],
                shadowSize: [41, 41]
            })
        }).addTo(map);
        marker.bindPopup(distance === 0 ? 'Vaga ocupada' : 'Vaga livre');

        // Inicializa o array de marcadores, se necessário
        if (!map.markers) {
            map.markers = [];
        }

        // Adiciona o novo marcador ao array
        map.markers[index] = marker;
    }
}

async function fetchData() {
    try {
        const response = await fetch('https://api.tago.io/data?variable=distance', {
            headers: {
                'Content-Type': 'application/json',
                'Device-Token': deviceToken
            }
        });

        if (!response.ok) {
            throw new Error('Network response was not ok');
        }

        const data = await response.json();
        const latestData = data.result; // Assumindo que os dados mais recentes estão no início da lista

        latestData.forEach((slotData, index) => {
            updateMarker(slotData, index);
        });
    } catch (error) {
        console.error('Fetch error:', error);
        document.querySelectorAll('.status').forEach(status => status.textContent = 'Erro ao buscar dados');
    }
}

function startFetchingData() {
    fetchData();
    setInterval(fetchData, 4000); // Atualiza os dados a cada 4 segundos
}

window.onload = startFetchingData;