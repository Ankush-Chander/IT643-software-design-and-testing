window.addEventListener('load', function() {
    // Track the active deck globally
    let activeDeck = null;

    document.querySelectorAll('.slides-deck').forEach(deck => {
        const slides = Array.from(deck.querySelectorAll('.slide'));
        const prevButton = deck.querySelector('.prev-slide');
        const nextButton = deck.querySelector('.next-slide');
        const progress = deck.querySelector('.slide-progress');
        const fullscreenBtn = deck.querySelector('.fullscreen-toggle');
        let currentIndex = 0;

        function showSlide(index) {
            slides.forEach(slide => slide.style.display = 'none');
            slides[index].style.display = 'block';
            progress.textContent = `${index + 1} / ${slides.length}`;
            prevButton.disabled = index === 0;
            nextButton.disabled = index === slides.length - 1;
        }

        function prevSlide() {
            if (currentIndex > 0) {
                currentIndex--;
                showSlide(currentIndex);
            }
        }

        function nextSlide() {
            if (currentIndex < slides.length - 1) {
                currentIndex++;
                showSlide(currentIndex);
            }
        }

        function toggleFullscreen() {
            deck.classList.toggle('fullscreen');
            if (deck.classList.contains('fullscreen')) {
                fullscreenBtn.textContent = '⛶';
                fullscreenBtn.title = 'Exit fullscreen';
            } else {
                fullscreenBtn.textContent = '⛶';
                fullscreenBtn.title = 'Enter fullscreen';
            }
            window.dispatchEvent(new Event('resize'));
        }

        // Set active deck on any interaction
        deck.addEventListener('mouseenter', () => {
            activeDeck = deck;
        });

        deck.addEventListener('click', () => {
            activeDeck = deck;
        });

        // Button click handlers
        prevButton.addEventListener('click', prevSlide);
        nextButton.addEventListener('click', nextSlide);
        fullscreenBtn.addEventListener('click', toggleFullscreen);

        // Keyboard navigation - moved to global handler
        showSlide(0);
    });

    // Global keyboard handler
    document.addEventListener('keydown', function(e) {
        if (!activeDeck) return;

        // Only handle keyboard if active deck is visible or in fullscreen
        const rect = activeDeck.getBoundingClientRect();
        const isVisible = (
            activeDeck.classList.contains('fullscreen') || 
            (rect.top >= 0 &&
            rect.left >= 0 &&
            rect.bottom <= window.innerHeight &&
            rect.right <= window.innerWidth)
        );
        
        if (!isVisible) return;

        const controls = {
            prevSlide: activeDeck.querySelector('.prev-slide'),
            nextSlide: activeDeck.querySelector('.next-slide')
        };

        switch (e.key) {
            case 'ArrowLeft':
            case 'PageUp':
                if (!controls.prevSlide.disabled) {
                    controls.prevSlide.click();
                    e.preventDefault();
                }
                break;
            case 'ArrowRight':
            case 'PageDown':
            case ' ': // Space
                if (!controls.nextSlide.disabled) {
                    controls.nextSlide.click();
                    e.preventDefault();
                }
                break;
            case 'Escape':
                if (activeDeck.classList.contains('fullscreen')) {
                    activeDeck.querySelector('.fullscreen-toggle').click();
                    e.preventDefault();
                }
                break;
        }
    });
}); 